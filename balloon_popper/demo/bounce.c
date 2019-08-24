#include "../include/body.h"
#include "../include/sdl_wrapper.h"
#include "../include/list.h"
#include "../include/vector.h"
#include "../include/utils.h"
#include "../include/scene.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
const Vector START_POINT = {0, 0};
const Vector START_VEL = {-100, 100.0};
const Vector START_ACC = {0, 0};
const Vector START_ELASTICITY = {1, 1};
const double ROTATE_ANGLE = M_PI/4;
const double RADIUS = 30.0;
const double RED = 0.75;
const double GREEN = 0.0;
const double BLUE = 0.75;
const double STARTING_MASS = 1.0;   // Not relevant to this demo
const Vector BOTTOM_LEFT_CORNER = {-500, -250};
const Vector TOP_RIGHT_CORNER = {500, 250};
const int NUM_ARMS = 5;

/**
* Loop through all the points of each body, if one of the points is past the
* window boundaries and the velocity isn't guiding away from the boundary,
* change the direction of the velocity. For example, if the star hits the top
* boundary and the velocity is positive, negate the sign of the velocity.
*/
void check_corner_bounds(Scene* scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        Body* star = scene_get_body(scene, i);
        check_out_of_bounds(star, BOTTOM_LEFT_CORNER, \
            true, double_less_then, START_ELASTICITY);
        check_out_of_bounds(star, TOP_RIGHT_CORNER, \
            true, double_great_then, START_ELASTICITY);
        check_out_of_bounds(star, BOTTOM_LEFT_CORNER,\
            false, double_less_then, START_ELASTICITY);
        check_out_of_bounds(star, TOP_RIGHT_CORNER, \
            false, double_great_then, START_ELASTICITY);
    }
}

/**
 * Initializes a scene which contains one star.
 * @return the initialized scene
 */
Scene* initialize_scene_bounce(void) {
    Scene *scene = scene_init();
    Vector center = START_POINT;
    List *star_points = get_star_points(NUM_ARMS, RADIUS, center);
    Body *star = body_init(star_points, STARTING_MASS, \
        (RGBColor){RED, GREEN, BLUE});
    body_set_velocity(star, START_VEL);
    body_set_acceleration(star, START_ACC);
    body_set_elasticity(star, START_ELASTICITY);
    scene_add_body(scene, star);
    return scene;
}

int main(int argc, char* argv[]) {
    sdl_init(BOTTOM_LEFT_CORNER, TOP_RIGHT_CORNER);
    Scene* scene = initialize_scene_bounce();
    Body* star = scene_get_body(scene, 0);
    /* Loop until window is x'd out of */
    while (!sdl_is_done()) {
        double dt = time_since_last_tick();
        check_corner_bounds(scene);
        body_set_rotation(star, dt * ROTATE_ANGLE + (body_get_angle(star)));
        scene_tick_no_forces(scene, dt);

        sdl_render_scene(scene);
    }
    scene_free(scene);
}
