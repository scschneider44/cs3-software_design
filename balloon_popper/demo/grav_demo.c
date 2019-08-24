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
const double RAD = 50.0;
const Vector START_ELASTICITY = {1, 1};    // Perfectly elastic horizontally
const double G = 10;
const Vector START_ACC = {0, 0};    // No X-acceleration
const int SMALLEST_RADIUS = 2;
const int LARGEST_RADIUS = 10;
const double TIME_SPACING = 2;
const double STARTING_MASS = 1.0;   // Not relevant for this demo
const int POINTS = 4;
const int NUM_BODIES = 50;

Scene* initialize_scene_grav(void) {
    Scene* scene = scene_init();
    for (size_t i = 0; i < NUM_BODIES; i++) {
        int radius = pseudo_rand_int(SMALLEST_RADIUS, LARGEST_RADIUS);
        Vector start_vel = {pseudo_rand_int(-10, 10), pseudo_rand_int(-10, 10)};

        List* star = get_star_points(POINTS, radius, rand_center(LENGTH_AND_HEIGHT));
        scene_add_special_body(scene, rand_color(), star, radius, start_vel, START_ACC, VEC_ZERO);
    }
    return scene;
}

void apply_gravitational_force(Scene *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    for(size_t j = i; j < scene_bodies(scene); j++) {
      create_newtonian_gravity(scene, G, scene_get_body(scene, i), \
                                         scene_get_body(scene, j));
    }
  }
}

int main(int argc, char* argv[]) {
    Vector bottom_left = vec_multiply(-0.5, LENGTH_AND_HEIGHT);
    Vector top_right = vec_multiply(0.5, LENGTH_AND_HEIGHT);

    sdl_init(bottom_left, top_right);
    Scene* scene = initialize_scene_grav();

    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        apply_gravitational_force(scene);

        scene_tick(scene, dt);

        sdl_render_scene(scene);
    }
    scene_free(scene);
}
