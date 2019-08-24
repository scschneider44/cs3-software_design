#include "../include/body.h"
#include "../include/sdl_wrapper.h"
#include "../include/list.h"
#include "../include/vector.h"
#include "../include/utils.h"
#include "../include/scene.h"
#include "../include/forces.h"
#include "../include/collision.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct gameInfo {
        Scene* scene;
} GameInfo;
/* screen dimensions */
const Vector LENGTH_AND_HEIGHT = {1000, 500};
const RGBColor GREEN = (RGBColor) {0, 1, 0};
const RGBColor GRAY = (RGBColor) {0.827, 0.827, 0.827};
const Vector ELASTICITY = {1, 1};
const double RADIUS_INVADERS = 40.0;
const double GAP = 3;             // Gap between rows of invaders
const size_t NUM_ROWS = 3;
const size_t NUM_COLS = 5;
const Vector INVADER_VELOCITY = {100, 0};
const Vector PLAYER_VELOCITY = {300, 0};
const Vector BULLET_VELOCITY = {0, -100};
const double PLAYER_HEIGHT = 40;
const double PLAYER_WIDTH = 80;
const double BULLET_HEIGHT = 15;
const double BULLET_WIDTH = 3;
const double SPAWN_INTERVAL = 1;

/**
 * Spawns invaders onto the scene
 * @param scene the scene
 */
void spawn_invaders(Scene *scene) {
    Vector top_left = (Vector){-LENGTH_AND_HEIGHT.x / 2, LENGTH_AND_HEIGHT.y / 2};
    double invader_diameter = 2 * RADIUS_INVADERS;
    for (size_t i = 0; i < NUM_ROWS; i++) {
        for (size_t j = 0; j < NUM_COLS; j++) {
            Vector invader_center = (Vector){top_left.x + \
                (RADIUS_INVADERS + invader_diameter * j), \
                top_left.y - (RADIUS_INVADERS + (RADIUS_INVADERS + GAP) * i)};
            List* invader_pts = get_partial_circle(RADIUS_INVADERS, 1, 5, \
                invader_center);
            Role *type = malloc(sizeof(Role));
            assert(type);
            *type = ENEMY;
            Body* invader = body_init_with_info(invader_pts, DEFAULT_MASS, GRAY, type, free);
            body_set_velocity(invader, INVADER_VELOCITY);
            scene_add_body(scene, invader);
        }
    }
}

/**
 * Ensures user cannot go off the horizontal sides of screen
 * @param scene the scene
 */
void keep_player_bounds(Scene* scene) {
    Body *player = scene_get_body(scene, 0);
    int wall_hit = which_wall_hit(player, LENGTH_AND_HEIGHT, false);
    if (wall_hit == RIGHT_WALL || wall_hit == LEFT_WALL) {
        body_set_velocity(player, VEC_ZERO);
    }
    return;
}

/**
 * Shifts invaders down a row once it hits a horizontal wal
 * @param body the invader to shift
 */
void shift_invaders_down(Body *body) {
    Vector centroid = body_get_centroid(body);
    body_set_centroid(body, vec_add(centroid, \
        (Vector){0, -(RADIUS_INVADERS + GAP) * 3}));
}

/**
 * Moves invaders
 * @param scene the scene
 * @param dt    the amount of time
 */
void move_invaders(Scene *scene, double dt) {
    for (size_t i = 1; i < scene_bodies(scene); i++) {
        Body *curr_body = scene_get_body(scene, i);
        if (body_get_role(curr_body) == ENEMY) {
            if(check_out_of_bounds(curr_body, vec_multiply(0.5, LENGTH_AND_HEIGHT), \
                true, double_great_then, ELASTICITY) ||
            check_out_of_bounds(curr_body, vec_multiply(-0.5, LENGTH_AND_HEIGHT), \
                true, double_less_then, ELASTICITY)) {
                    shift_invaders_down(curr_body);
            }
        }
    }
}

/**
 * Spawns destructive forces between bullets and other bodies
 * @param scene    the scene
 * @param is_alien whether or not the bullet spawned is from an alien
 * @param bullet   the bullet
 */
void spawn_destructive_force(Scene* scene, bool is_alien, Body* bullet) {
    Body* player = scene_get_body(scene, 0);
    if (is_alien) {
        create_destructive_collision(scene, bullet, player);
    } else {
        for (size_t i = 1; i < scene_bodies(scene); i++) {
            Body *curr_body = scene_get_body(scene, i);
            if (body_get_role(curr_body) == ENEMY) {
                create_destructive_collision(scene, bullet, curr_body);
            }
        }
    }
}

/**
 * Spawns a bullet onto the scene
 * @param scene    the scene
 * @param is_alien whether to spawn a bullet from an alien or not
 */
void spawn_bullet(Scene *scene, bool is_alien) {
    Body *player_body = scene_get_body(scene, 0);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = BULLET;
    List *points;
    if (is_alien) {
        double smallest_dist = LENGTH_AND_HEIGHT.x;
        Body *closest_body;
        for (size_t i = 1; i < scene_bodies(scene); i++) {
            Body *curr_body = scene_get_body(scene, i);
            if (body_get_role(curr_body) == ENEMY) {
                if (fabs(body_get_centroid(curr_body).x - \
                    body_get_centroid(player_body).x) < smallest_dist) {
                    smallest_dist = fabs(body_get_centroid(curr_body).x - \
                    body_get_centroid(player_body).x);
                    closest_body = curr_body;
                }
            }
        }
        points = get_oval_points(body_get_centroid(closest_body), \
            BULLET_WIDTH, BULLET_HEIGHT);
    } else {
        points = get_oval_points(body_get_centroid(player_body), \
            BULLET_WIDTH, BULLET_HEIGHT);
    }

    Body *bullet;
    if (is_alien) {
        bullet = body_init_with_info(points, DEFAULT_MASS, GRAY, type, free);
        body_set_velocity(bullet, BULLET_VELOCITY);
    } else {
        bullet = body_init_with_info(points, DEFAULT_MASS, GREEN, type, free);
        body_set_velocity(bullet, vec_multiply(-1, BULLET_VELOCITY));
    }
    spawn_destructive_force(scene, is_alien, bullet);
    scene_add_body(scene, bullet);
}

/**
 * Key handler for handling user keyboard input
 * @param key       the key pressed
 * @param type      the type (key KEY_RELEASED, KEY_PRESSED)
 * @param held_time the amount of time the key was held down
 * @param info      a pointer to any additional information the key handler
 *                  may need
 */
void on_key(char key, KeyEventType type, double held_time, void* info) {
    GameInfo* i = info;
    Body *player = scene_get_body(i->scene, 0);
    int wall_hit = which_wall_hit(player, LENGTH_AND_HEIGHT, false);
    if (type == KEY_RELEASED) {
        body_set_velocity(player, VEC_ZERO);
    } else if (type == KEY_PRESSED) {
        switch (key) {
            case RIGHT_ARROW:
                if (wall_hit != RIGHT_WALL)
                    body_set_velocity(player, PLAYER_VELOCITY);
                break;
            case LEFT_ARROW:
                if (wall_hit != LEFT_WALL)
                    body_set_velocity(player, vec_multiply(-1, PLAYER_VELOCITY));
                break;
            case ' ':
                spawn_bullet(i->scene, false);
                break;
        }
    }
}

/**
 * Spawns a player onto the scene
 * @param scene the scene to spawn the player onto
 */
void spawn_player(Scene *scene) {
    List* points = get_oval_points(
            (Vector){0, -LENGTH_AND_HEIGHT.y / 2 + PLAYER_HEIGHT / 2},
            PLAYER_WIDTH, PLAYER_HEIGHT);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = PLAYER;
    Body* player = body_init_with_info(points, DEFAULT_MASS, GREEN, type, free);
    // Player starts out still
    body_set_velocity(player, VEC_ZERO);
    scene_add_body(scene, player);
}

/**
 * Destroys/removes a bullet from the scene when it goes off stage
 * @param scene the scene
 */
void destroy_bullet(Scene *scene) {
    for (size_t i = 1; i < scene_bodies(scene); i++) {
        Body *body = scene_get_body(scene, i);
        if (body_get_role(body) == BULLET) {
            if (fabs(body_get_centroid(body).y) > (LENGTH_AND_HEIGHT.y * 0.5)) {
                body_remove(body);
            }
        }
    }
}

/**
 * Spawns an alien bullet every time interval
 * @param time_since_last_spawn the time since the last spawn
 * @param scene                 the scene
 */
void spawn_alien_bullet(double* time_since_last_spawn, Scene* scene) {
    if (*time_since_last_spawn > SPAWN_INTERVAL) {
        spawn_bullet(scene, true);
        *time_since_last_spawn = 0;
    }
}


int game_is_over(Scene *scene) {
    if (body_get_role(scene_get_body(scene, 0)) != PLAYER) {
        return 1;
    }

    int invader_count = 0;

    for (size_t i = 1; i < scene_bodies(scene); i++) {
        Body *curr_body = scene_get_body(scene, i);
        if (body_get_role(curr_body) == ENEMY) {
            if (body_get_centroid(curr_body).y - RADIUS_INVADERS < -LENGTH_AND_HEIGHT.y / 2) {
                return 1;
            }

            invader_count++;
        }
    }

    if (invader_count == 0) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    initialize_window(LENGTH_AND_HEIGHT);
    Scene* scene = initialize_scene();
    GameInfo* gameInfo = malloc(sizeof(GameInfo));
    assert(gameInfo);
    gameInfo->scene = scene;
    sdl_on_key(on_key, gameInfo);
    double dt;
    double time_elapsed = 0;

    spawn_player(scene);
    spawn_invaders(scene);
    while (!sdl_is_done() && !game_is_over(scene)) {
        dt = time_since_last_tick();
        time_elapsed += dt;

        spawn_alien_bullet(&time_elapsed, scene);

        destroy_bullet(scene);
        move_invaders(scene, dt);
        scene_tick(scene, dt);
        scene_tick_no_forces(scene, dt);
        keep_player_bounds(scene);
        sdl_render_scene(scene);
    }
    scene_free(scene);
    free(gameInfo);
    return 0;
}
