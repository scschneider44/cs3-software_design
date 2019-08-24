#include "../include/body.h"
#include "../include/sdl_wrapper.h"
#include "../include/list.h"
#include "../include/vector.h"
#include "../include/utils.h"
#include "../include/scene.h"
#include "../include/forces.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* screen dimensions */
const Vector LENGTH_AND_HEIGHT = {1000, 500};
const Vector START_VEL = {110, 0};
const double ROTATE_ANGLE = M_PI/4;
const double RAD = 50.0;
const Vector START_ELASTICITY = {1, 0.7};    // Perfectly elastic horizontally
const double GRAVITY = -98.1;
const Vector GRAV_ACC = {0, GRAVITY};    // No X-acceleration
const int FEWEST_POINTS = 3;
const int MOST_POINTS = 7;
const double TIME_SPACING = 1;
const double STARTING_MASS = .05;   // Not relevant for this demo

/**
 * Rotates each body in the scene by a certain angle
 * @param scene the scene
 * @param dt the time since the last tick
 */
void rotate_bodies(Scene* scene, double dt) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        Body* star = scene_get_body(scene, i);
        // Our new abstractions use absolute angles
        body_set_rotation(star, dt * ROTATE_ANGLE + (body_get_angle(star)));
    }
}

/**
 * Returns 1 if a shape is going off the right end of the screen
 * @param  body the shape
 * @return      1 if is going off, 0 otherwise
 */
bool is_out_of_frame(Body* body) {
    return (body_get_centroid(body).x - RAD > \
        (LENGTH_AND_HEIGHT.x * 0.5));
}

/**
 * Handles the bounds checking. It removes bodies that are off the right
 * end of the screen, and adjusts for bouncing off the ground.
 * @param scene the scene
 */
void check_bounds(Scene* scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        Body* star = scene_get_body(scene, i);
        if (is_out_of_frame(star)) {
            // Removes stars that are off to the right
            scene_remove_body(scene, i);
        } else {
            // Bounces stars off the ground
            check_out_of_bounds(star, vec_multiply(-0.5, LENGTH_AND_HEIGHT),\
                false, double_less_then, START_ELASTICITY);
        }
    }
}

int main(int argc, char* argv[]) {
    Vector bottom_left = vec_multiply(-0.5, LENGTH_AND_HEIGHT);
    Vector top_right = vec_multiply(0.5, LENGTH_AND_HEIGHT);
    Vector drop_point = {LENGTH_AND_HEIGHT.x * -.5 + RAD, \
        LENGTH_AND_HEIGHT.y * .5 - RAD};

    sdl_init(bottom_left, top_right);
    Scene* scene = scene_init();

    // + 1 ensures we spawn a shape immediately
    double time_since_last_star = TIME_SPACING + 1;

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        time_since_last_star += dt;

        if (time_since_last_star > TIME_SPACING) {
            int num_points = pseudo_rand_int(FEWEST_POINTS, MOST_POINTS);
            List* star = get_star_points(num_points, RAD, drop_point);
            scene_add_special_body(scene, rand_color(), star, -1, START_VEL, GRAV_ACC, START_ELASTICITY);
            time_since_last_star = 0;
        }
        // grav(scene);
        check_bounds(scene);
        // Rotate and translate bodies
        rotate_bodies(scene,dt);


        scene_tick_no_forces(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}
