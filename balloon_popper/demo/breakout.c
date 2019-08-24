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
#define LENGTH_AND_HEIGHT (Vector){1000, 500}
const RGBColor RED = (RGBColor) {1, 0, 0};
const RGBColor ORANGE = (RGBColor) {1, 127.0/255, 0};
const RGBColor YELLOW = (RGBColor) {1, 1, 0};
const RGBColor GREEN = (RGBColor) {0, 1, 0};
const RGBColor BLUE = (RGBColor) {0, 0, 1};
const RGBColor INDIGO = (RGBColor) {39.0/255, 0, 51.0/255};
const RGBColor VIOLET = (RGBColor) {139.0/255, 0, 1};

const Vector ELASTICITY = {1, 1};
const double GAP = 5;             // Gap between blocks
const int NUM_ROWS = 3;
const int NUM_COLS = 7;
const double BLOCK_WIDTH = (LENGTH_AND_HEIGHT.x - (NUM_COLS + 1) * GAP) / NUM_COLS;
const double BLOCK_HEIGHT = 20;
const Vector BALL_VELOCITY = {200, 200};
const Vector PLAYER_VELOCITY = {500, 0};
const double BALL_RADIUS = 20;
const double BALL_MASS = 20;

void spawn_blocks(Scene *scene) {
    const RGBColor RAINBOW_COLORS[7] = {RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET};
    Vector top_left = (Vector){-LENGTH_AND_HEIGHT.x / 2, LENGTH_AND_HEIGHT.y / 2};
    for (size_t i = 0; i < NUM_ROWS; i++) {
        double y_coord = top_left.y - GAP - (BLOCK_HEIGHT + GAP) * i - BLOCK_HEIGHT / 2;
        for (size_t j = 0; j < NUM_COLS; j++) {
            double op_block = pseudo_rand_decimal(0, 1);
            Vector block_center = (Vector){GAP + top_left.x + \
                (BLOCK_WIDTH + GAP) * j + BLOCK_WIDTH / 2, y_coord};
            List* block_pts = get_rectangle(block_center, BLOCK_WIDTH, BLOCK_HEIGHT);
            Role *type = malloc(sizeof(Role));
            assert(type);
            if (op_block < .5) {
              *type = TURN_WHITE_ON_COLLISION;
            }
            else {
              *type = REMOVE_ON_COLLISION;
            }
            Body* block = body_init_with_info(block_pts, INFINITY, RAINBOW_COLORS[j], type, free);
            body_set_velocity(block, VEC_ZERO);
            scene_add_body(scene, block);
        }
    }
}

/**
 * Deletes all enemy blocks from the scene
 */
void delete_blocks(Scene *scene) {
  for (size_t i = 2; i < scene_bodies(scene); i++) {
      body_remove(scene_get_body(scene, i));
  }
}

/*
 * Spawns ball onto the scene
 * @param scene the scene
 */
void spawn_ball(Scene *scene) {
    Vector ball_center = (Vector) {0, -LENGTH_AND_HEIGHT.y / 2 +
        BLOCK_HEIGHT + BALL_RADIUS};
    List* ball_pts = get_oval_points(ball_center, BALL_RADIUS, BALL_RADIUS);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = BULLET;
    Body* ball = body_init_with_info(ball_pts, BALL_MASS, RED, type, free);
    body_set_velocity(ball, BALL_VELOCITY);
    scene_add_body(scene, ball);
}

void spawn_player(Scene *scene) {
    // Player will start at bottom of screen, in the middle
    Vector player_center = (Vector){0, -LENGTH_AND_HEIGHT.y / 2 + GAP + BLOCK_HEIGHT / 2};
    List* player_pts = get_rectangle(player_center, BLOCK_WIDTH, BLOCK_HEIGHT);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = PLAYER;
    Body* player = body_init_with_info(player_pts, INFINITY, RED, type, free);
    // Player will start out stationary
    body_set_velocity(player, VEC_ZERO);
    scene_add_body(scene, player);
}

/**
 * Spawns destructive forces between ball and enemy blocks
 * @param scene    the scene
 * @param ball   the ball
 */
void spawn_physics_collisions(Scene* scene, Body* ball, size_t idx_start) {
    for (size_t i = idx_start; i < scene_bodies(scene); i++) {
        Body *curr_body = scene_get_body(scene, i);
        if (body_get_role(curr_body) != BULLET) {
          create_physics_collision(scene, ELASTICITY.y, ball, curr_body);
        }
    }
}

/**
 * Restarts the game if the ball hits the bottom wall by putting the player
 * and ball back to their starting positions and deleting and resetting all of
 * the enemy blocks.
 * @param scene     the scene
 * @param player    the player
 * @param ball      the ball
 */
void restart_game(Scene *scene, Body *player, Body *ball) {

    body_set_centroid(player, (Vector){0, -LENGTH_AND_HEIGHT.y / 2 + GAP + BLOCK_HEIGHT / 2});
    body_set_centroid(ball, (Vector){0, -LENGTH_AND_HEIGHT.y / 2 + 10 * GAP +
      BLOCK_HEIGHT / 2});
    body_set_velocity(ball, BALL_VELOCITY);
    delete_blocks(scene);
    spawn_blocks(scene);
    spawn_physics_collisions(scene, ball, 2);
    body_set_velocity(player, VEC_ZERO);
}

/**
 * Ends the game if all of the enemy blocks have been destroyed
 * @param scene   the scene
 */
bool game_is_over(Scene *scene) {
  if (scene_bodies(scene) <= 2) {
    return true;
  }
  return false;
}

/**
* Loop through all the points of each body, if one of the points is past the
* window boundaries and the velocity isn't guiding away from the boundary,
* change the direction of the velocity.
* If the ball hits the bottom wall then we will have to restart the game
*/
void check_corner_bounds(Scene* scene) {
        Body *player = scene_get_body(scene, 0);
        Body *ball = scene_get_body(scene, 1);
        check_out_of_bounds(ball, vec_multiply(-0.5, LENGTH_AND_HEIGHT), \
            true, double_less_then, ELASTICITY);
        check_out_of_bounds(ball, vec_multiply(0.5, LENGTH_AND_HEIGHT), \
            true, double_great_then, ELASTICITY);
        check_out_of_bounds(ball, vec_multiply(0.5, LENGTH_AND_HEIGHT), \
            false, double_great_then, ELASTICITY);
        if (check_out_of_bounds(ball, vec_multiply(-0.5, LENGTH_AND_HEIGHT), \
                false, double_less_then, ELASTICITY)) {
                  restart_game(scene, player, ball);
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
                // TODO: Add spawning of ball here
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    initialize_window(LENGTH_AND_HEIGHT);
    Scene* scene = initialize_scene();
    spawn_player(scene);
    spawn_ball(scene);
    spawn_blocks(scene);
    spawn_physics_collisions(scene, scene_get_body(scene, 1), 0);
    GameInfo* gameInfo = malloc(sizeof(GameInfo));
    assert(gameInfo);
    gameInfo->scene = scene;
    sdl_on_key(on_key, gameInfo);
    double dt;

    while (!sdl_is_done() && !game_is_over(scene)) {
        dt = time_since_last_tick();
        body_set_time_since_last_collision(scene_get_body(scene, 1), \
            body_get_time_since_last_collision(scene_get_body(scene, 1)) + dt);
        scene_tick(scene, dt);
        scene_tick_no_forces(scene, dt);
        keep_player_bounds(scene);
        check_corner_bounds(scene);
        sdl_render_scene(scene);
    }
    scene_free(scene);
    free(gameInfo);
    return 0;
}
