#include "body.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include "utils.h"
#include "scene.h"
#include "forces.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Screen dimensions */
const Vector LENGTH_AND_HEIGHT = {1000, 500};
const double MASS = 1.0;
const double CIRCLE_RADIUS = 10.0;
const RGBColor WHITE = (RGBColor) {1, 1, 1};
const double SPRING_CONSTANT = 2;
const double DRAG_COEFFICIENT = .3;

/**
 * Initializes the scene. Adds Pacman and a preset number of pellets.
 * @return  the initalized scene
 */
Scene* initialize_scene_spring(void) {
    Scene* scene = scene_init();

    double num_of_circles = vec_magnitude(LENGTH_AND_HEIGHT) / (2 * CIRCLE_RADIUS);
    double center_x = -(LENGTH_AND_HEIGHT.x / 2);
    double center_y = -(LENGTH_AND_HEIGHT.y / 2);
    double slope = LENGTH_AND_HEIGHT.y / num_of_circles;

    for (size_t i = 0; i <= num_of_circles; i++) {
        List* circle1 = get_circle_points((Vector){center_x, -center_y}, CIRCLE_RADIUS);
        List* circle2 = get_circle_points((Vector){center_x, center_y}, CIRCLE_RADIUS);
        scene_add_special_body(scene, WHITE, circle1, -1, VEC_ZERO, VEC_ZERO, VEC_ZERO);
        scene_add_special_body(scene, rand_color(), circle2, -1, VEC_ZERO, VEC_ZERO, VEC_ZERO);
        center_x += (CIRCLE_RADIUS * 2);
        center_y += slope;
    }
    return scene;
}

int main(int argc, char* argv[]) {
    Vector bottom_left = vec_multiply(-0.5, LENGTH_AND_HEIGHT);
    Vector top_right = vec_multiply(0.5, LENGTH_AND_HEIGHT);

    sdl_init(bottom_left, top_right);
    Scene* scene = initialize_scene_spring();

    for (size_t i = 0; i < scene_bodies(scene); i += 2) {
        Body *body1 = scene_get_body(scene, i);
        Body *body2 = scene_get_body(scene, i+1);
        create_spring(scene, SPRING_CONSTANT, body1, body2);

        if (i < scene_bodies(scene) / 2) {
            create_drag(scene, DRAG_COEFFICIENT, body2);
        }
    }

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();

        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}
