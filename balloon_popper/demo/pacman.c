#include "../include/body.h"
#include "../include/sdl_wrapper.h"
#include "../include/list.h"
#include "../include/vector.h"
#include "../include/utils.h"
#include "../include/scene.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* screen dimensions */
const Vector LENGTH_AND_HEIGHT = {1000, 500};
const RGBColor YELLOW = (RGBColor) {.8588, .8863, .1098};
const double MASS = 1.0;       // Mass not accounted for in demo
const double SPAWN_INTERVAL = 3.0;      // Pellets spawn every 5 seconds
const double RADIUS_PACMAN = 60.0;
const double RADIUS_PELLET = 10.0;
const size_t INITIAL_PELLETS = 10;
const double JERK = 200.0;        // Acceleration increase per second
const double RESET_VELOCITY = 120;   // Velocity to set when changing direction
Body* pacman = NULL;

/**
 * Returns points of a pacman.
 * @return  list of (x,y) coordinates for pacman
 */
List* get_pacman_points(void) {
    size_t number_pts = 50;        // Arbitrarily large number for Pacman
    List* points = list_init(number_pts, vector_free);
    double angle = 2 * M_PI / number_pts;

    Vector center = {0, 0};
    list_add(points, create_vector_p(center));
    /* We do not initialize 1/6 of the circle to make room for the wedge
     * or mouth of Pacman. */
    for (size_t i = number_pts / 12; i < 11 * number_pts / 12; i++) {
        Vector vertex = {center.x + RADIUS_PACMAN * cos(i * angle), center.y + \
            RADIUS_PACMAN *sin(i * angle)};
        list_add(points , create_vector_p(vertex));
    }
    return points;
}

/**
 * Returns which wall Pacman will hit if it hits a wall, else 0
 * @return  the wall, or 0 if it will not run into a wall
 */
int return_wall_hit(void) {
    Vector centroid = body_get_centroid(pacman);
    if (centroid.x - RADIUS_PACMAN > (LENGTH_AND_HEIGHT.x * 0.5)) {
        return RIGHT_WALL;
    } else if (centroid.x + RADIUS_PACMAN < (LENGTH_AND_HEIGHT.x * -0.5)) {
        return LEFT_WALL;
    } else if (centroid.y - RADIUS_PACMAN > (LENGTH_AND_HEIGHT.y * 0.5)) {
        return TOP_WALL;
    } else if (centroid.y + RADIUS_PACMAN < (LENGTH_AND_HEIGHT.y * -0.5)) {
        return BOTTOM_WALL;
    }
    return 0;
}

/**
 * Handles the wrap around of Pacman. For instance, if Pacman goes off the
 * right end of the screen, it should come in from the left end of the screen.
 */
void wrap_around(void) {
    switch(return_wall_hit()) {
        /*
         * If we hit a wall, we simply want it to move to the opposite wall.
         * We actually want the centroid of Pacman to start 'behind' the wall.
         * In other words, we want the edge of Pacman to be touching the
         * "outside" of the wall.
         */
        case RIGHT_WALL:
            body_translate(pacman, \
                (Vector){-2 * (LENGTH_AND_HEIGHT.x * 0.5 + RADIUS_PACMAN), 0});
            break;
        case LEFT_WALL:
            body_translate(pacman, \
                (Vector){2 * (LENGTH_AND_HEIGHT.x * 0.5 + RADIUS_PACMAN), 0});
            break;
        case TOP_WALL:
            body_translate(pacman, \
                (Vector){0, -2 * (LENGTH_AND_HEIGHT.y * 0.5 + RADIUS_PACMAN)});
            break;
        case BOTTOM_WALL:
            body_translate(pacman, \
                (Vector){0, 2 * (LENGTH_AND_HEIGHT.y * 0.5 + RADIUS_PACMAN)});
            break;
    }
}
/**
 * Initializes pacman by adding him to the center of the scene.
 * @param scene the scene to add pacman to
 */
void add_pacman_to_scene(Scene* scene) {
    assert(scene);
    // Pacman will start out still
    List* points = get_partial_circle(RADIUS_PACMAN, 1, 11, VEC_ZERO);
    pacman = body_init(points, MASS, YELLOW);
    body_set_velocity(pacman, VEC_ZERO);
    body_set_acceleration(pacman, VEC_ZERO);
    scene_add_body(scene, pacman);
}

/**
 * Returns true if pacman is eating a pellet. The logic for it is as follows:
 *
 *                                   c (centroid of pellet of radius r)
 *                                   |
 *                                   |
 *    (centroid of pacman) a---------d-----b   (outer edge of Pacman)
 *
 * The goal is to check if a pellet with centroid 'c' will come into contact
 * with two line segments (one for each edge of Pacman's mouth).
 * To check if a pellet with centroid 'c' touches a line segment 'ab', we
 * simply project 'ac' onto 'ab' to get segment 'ad'. Now, to see if the pellet
 * is touching segment 'ab', the following two conditions must hold:
 *
 * 1. |'cd'| <= r. In other words, the distance between the center of the pellet
 * and the point 'd' which marks the end of the projection is less than or equal
 * to r.
 *
 * 2. |'ad'| <= |'ab'|. This ensures that the pellet is not too far horizontal
 * wise. This ensures that the following case will not falsely identify the
 * pellet touching the segment 'ab'.
 *
 *                                                        c (centroid of pellet)
 *                                                        |
 *                                                        |
 *(centroid of pacman) a-----b (outer edge of Pacman)-----d
 *
 * @param  current_pellet the pellet to examine
 * @return                1 if the pellet is going to be eaten, else 0
 */
int is_eating_pellet(Body* current_pellet) {
    assert(current_pellet);
    List* points = body_get_shape(pacman);
    Vector centroid = body_get_centroid(pacman);

    /* We know that the two points connecting the centroid to the
     * 'circumference' of the circle are the 2nd point added and last
     * point added to the points of Pacman. */
    Vector mouth_segment_1 = vec_subtract(*(Vector*)list_get(points, 1), \
        centroid);
    double magnitude_mouth_segment_1 = vec_magnitude(mouth_segment_1);

    Vector mouth_segment_2 = vec_subtract(*(Vector*)list_get(points, \
        list_size(points) - 1), centroid);
    double magnitude_mouth_segment_2 = vec_magnitude(mouth_segment_2);

    Vector pellet_centroid = body_get_centroid(current_pellet);

    // Stores the vector from the centroid of pacman to that of a pellet
    Vector pacman_to_pellet = vec_subtract(pellet_centroid, centroid);
    double magnitude_pacman_to_pellet = vec_magnitude(pacman_to_pellet);

    // Project onto the first line segment of "mouth"
    double magnitude_projection_1 = vec_magnitude( \
        vec_projection(pacman_to_pellet, mouth_segment_1));
    /* distance stores distance between center of pellet and point
     * marking the end of the projection. */
    double distance_1 = sqrt(magnitude_pacman_to_pellet * \
        magnitude_pacman_to_pellet - magnitude_projection_1 * \
        magnitude_projection_1);

    // Project onto second line segment of "mouth"
    double magnitude_projection_2 = vec_magnitude( \
        vec_projection(pacman_to_pellet, mouth_segment_2));
    double distance_2 = sqrt(magnitude_pacman_to_pellet * \
        magnitude_pacman_to_pellet - magnitude_projection_2 * \
        magnitude_projection_2);

    if ((magnitude_projection_1 <= magnitude_mouth_segment_1 && distance_1 \
        <= RADIUS_PELLET) || (magnitude_projection_2 <= \
            magnitude_mouth_segment_2 && distance_2 <= RADIUS_PELLET)) {
            return 1;
    }
    return 0;
}

/**
 * Removes pellets from the scene that have come into contact with Pacman
 * @param scene the scene
 */
void remove_pellets_on_contact(Scene *scene) {

    Body* current_pellet;
    size_t num_bodies = scene_bodies(scene);
    /* Iterate through each pellet and see if it's touching
     * Pacman. If so, remove it. Note that we don't look at index 0 of bodies
     * because scene_get_body(scene, 0) is Pacman. */
    for (size_t i = num_bodies - 1; i > 0; i--) {
        current_pellet = scene_get_body(scene, i);
        if (is_eating_pellet(current_pellet)) {
            scene_remove_body(scene, i);
        }
    }
}

void arrow_pressed(double held_time, double angle, int is_vertical) {
    Vector current_acceleration = body_get_acceleration(pacman);
    Vector current_velocity = body_get_velocity(pacman);
    /* Sign stores whether or not to increase or decrease acceleration and
     * velocity. If angle is negative (- PI or -PI/2), then we set sign to
     * -1 because we need to decrease because we are going downwards (if y) or
     * leftwards (if x). */
    int sign = angle < 0 ? -1 : 1;
    /**
     * If we change directions, we immediately halt Pacman and give
     * Pacman the reset_velocity in the direction of the pressed key.
     * Then, we adjust the acceleration proportional to how long
     * the key has been pressed.
     */
    if (body_get_angle(pacman) != angle) {
        body_set_rotation(pacman, angle);
        current_velocity = VEC_ZERO;
        if (is_vertical) {
            current_velocity.y = sign * RESET_VELOCITY;
        } else {
            current_velocity.x = sign * RESET_VELOCITY;
        }
        body_set_velocity(pacman, current_velocity);
    }
    if (is_vertical) {
        current_acceleration.y += sign * JERK * held_time;
    } else {
        current_acceleration.x += sign * JERK * held_time;
    }
    body_set_acceleration(pacman, current_acceleration);

}
/**
 * Handles key presses from the user. Arrows keys change the direction of
 * Pacman and the longer the key is pressed, the more Pacman accelerates
 * @param key       the key
 * @param type      type of action (press/release)
 * @param held_time the amount of time a key is held (seconds)
 */
void on_key(char key, KeyEventType type, double held_time, void* info) {
    Vector current_acceleration = body_get_acceleration(pacman);
    Vector current_velocity = body_get_velocity(pacman);
    /* Upon release of a key, we reset the acceleration to zero and
     * the velocity to RESET_VELOCITY so users can maintain control
     * after holding down a key. */
    if (type == KEY_RELEASED) {
        current_acceleration = VEC_ZERO;
        current_velocity = VEC_ZERO;

        switch (key) {
            case UP_ARROW:
                current_velocity.y = RESET_VELOCITY;
                break;
            case DOWN_ARROW:
                current_velocity.y = -RESET_VELOCITY;
                break;
            case RIGHT_ARROW:
                current_velocity.x = RESET_VELOCITY;
                break;
            case LEFT_ARROW:
                current_velocity.x = -RESET_VELOCITY;
                break;
        }

        body_set_velocity(pacman, current_velocity);
        body_set_acceleration(pacman, current_acceleration);
    }
    if (type == KEY_PRESSED) {
        switch (key) {
            case UP_ARROW:
                arrow_pressed(held_time, M_PI/2, 1);
                break;
            case DOWN_ARROW:
                arrow_pressed(held_time, -M_PI/2, 1);
                break;
            case RIGHT_ARROW:
                arrow_pressed(held_time, 0, 0);
                break;
            case LEFT_ARROW:
                arrow_pressed(held_time, -M_PI, 0);
                break;
        }
    }
}

/**
 * actually puts a pellet on the screen
 */
void add_pellet_to_scene(Scene *scene) {
    Vector center = rand_center(LENGTH_AND_HEIGHT);
    List* circle_points = get_circle_points(center, RADIUS_PELLET);
    scene_add_special_body(scene, YELLOW, circle_points, -1, VEC_ZERO, VEC_ZERO, VEC_ZERO);
}

/**
 * checks to see if it is time to spawn a pellet every SPAWN_INTERVAL and does if need be
 * @param time_since_last_spawn time (secs) since the last pellet was spawned
 * @param scene                 the scene
 */
void spawn_pellet(double* time_since_last_spawn, Scene* scene) {
    if (*time_since_last_spawn > SPAWN_INTERVAL) {
        add_pellet_to_scene(scene);
        *time_since_last_spawn = 0;
    }
}


int main(int argc, char* argv[]) {

    // Register key handler for up, down, left, right arrow keys
    sdl_on_key(on_key, NULL);
    initialize_window(LENGTH_AND_HEIGHT);

    Scene *scene = initialize_scene();

    // Variable helps us spawn a pellet every SPAWN_INTERVAL
    double time_since_last_spawn = 0;

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        time_since_last_spawn += dt;

        spawn_pellet(&time_since_last_spawn, scene);

        wrap_around();
        remove_pellets_on_contact(scene);
        scene_tick_no_forces(scene, dt);

        sdl_render_scene(scene);
    }
    scene_free(scene);
    return 0;
}
