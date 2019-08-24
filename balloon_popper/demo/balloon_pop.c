#include "../include/body.h"
#include "../include/sdl_wrapper.h"
#include "../include/list.h"
#include "../include/vector.h"
#include "../include/utils.h"
#include "../include/scene.h"
#include "../include/forces.h"
#include "../include/collision.h"
#include "../include/sprite.h"
#include "../include/game_info.h"
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

char* PLAYER_SPRITE_PATH = "images/Archer.bmp\0";

const RGBColor BLACK = (RGBColor) {0, 0, 0};

// screen dimensions
#define LENGTH_AND_HEIGHT (Vector){1000, 500}

// Constants for gravity
#define G 6.67E-11 // N m^2 / kg^2
#define M 6E24 // kg
#define g 9.8 // m / s^2
#define R (sqrt(G * M / g)) // m

const RGBColor RED = (RGBColor) {1, 0, 0};
const RGBColor ORANGE = (RGBColor) {1, 127.0/255, 0};
const RGBColor YELLOW = (RGBColor) {1, 1, 0};
const RGBColor GREEN = (RGBColor) {0, 1, 0};
const RGBColor BLUE = (RGBColor) {0, 0, 1};
const RGBColor INDIGO = (RGBColor) {39.0/255, 0, 51.0/255};
const RGBColor VIOLET = (RGBColor) {139.0/255, 0, 1};
const RGBColor WHITE = (RGBColor) {1, 1, 1};
const Vector OUTER_BAR_CENTER = {-375, -225};
const double OUTER_BAR_WIDTH = 100;
const double BAR_HEIGHT = 20;
const int POWER_DIVISIONS = 100;
const int BALLOON_IDX_START = POWER_DIVISIONS+3;

const double ARROW_WIDTH = 60;
const double ARROW_HEIGHT = 4;
const double ARROW_LENGTH = 5;
const double ARROW_HOLD_TIME = 0.05;
const Vector ARCHER_POSITION = (Vector){-400, -100};
const double POWER_BAR_TIME = 3;

// Dimensions of image are 346x490
const int ARCHER_WIDTH = 346/5;
const int ARCHER_HEIGHT = 490/5;

const Vector ELASTICITY = {1, 1};
const double BUFFER = 100;
const double GAP = 5;             // Gap between balloons
const int NUM_ROWS1 = 6;
const int NUM_COLS1 = 7;
const int NUM_ROWS2 = 8;
const int NUM_COLS2 = 8;
const int NUM_ROWS3 = 6;
const int NUM_COLS3 = 7;

const double BALLOON_WIDTH = 29;
const double BALLOON_HEIGHT = 35;

const double DART_LENGTH = 18;
const double DART_THICKNESS = 1.5;
const double DART_MASS = 20.0;       // kg
const double MAX_DART_VELOCITY = 300;

const double ROTATE_ANGLE = M_PI/4;

char* FONT_TYPE = "fonts/Game_font.ttf\0";
const int FONT_SIZE = 12;
const int SCORE_TEXT_WIDTH = 320;
const int SCORE_TEXT_HEIGHT = 30;
const Vector SCORE_TEXT_POSITION = {-160, -215};

typedef struct {
    double power;
    int level;
    int dartsLeft;
    int target;
} AdditionalInfo;

void free_additional_info(void* data) {
    AdditionalInfo* i = data;
    free(i);
}

bool no_darts_on_screen(Scene *scene) {
    if (body_get_role(scene_get_body(scene, scene_bodies(scene) - 1)) \
        == REMOVE_ON_COLLISION) {
            return true;
    }
    return false;
}

void initialize_power_bars(GameInfo* game_info) {
    Scene* scene = get_scene(game_info);
    Vector outer_bar_center = {ARCHER_POSITION.x + 25, ARCHER_POSITION.y - 103};
    double outer_bar_left = outer_bar_center.x - (OUTER_BAR_WIDTH / 2);
    double bar_width = (OUTER_BAR_WIDTH / POWER_DIVISIONS);
    Vector center = vec_init(outer_bar_left + (bar_width / 2), outer_bar_center.y);

    for (size_t i = 0; i < POWER_DIVISIONS; i++) {
        List *bar_points = get_rectangle(center, bar_width, BAR_HEIGHT);
        scene_add_special_body(scene, BLACK, bar_points, -1, VEC_ZERO, VEC_ZERO, VEC_ZERO);
        center = vec_add(center, (Vector){bar_width, 0});
    }
}

void update_power_bars(GameInfo* game_info) {
    Scene* scene = get_scene(game_info);
    double power = ((AdditionalInfo*)(get_additional_info(game_info)))->power;
    for (size_t i = 1; i <= POWER_DIVISIONS; i++) {
        Body *bar_to_color = scene_get_body(scene, i-1);
        if (i <= power * POWER_DIVISIONS) {
            body_set_color(bar_to_color, RED);
        } else {
            body_set_color(bar_to_color, BLACK);
        }
    }
}
/*
 * Spawns arrow onto the scene
 * @param scene the scene
 */
void spawn_arrow(GameInfo *game_info) {
    Scene* scene = get_scene(game_info);
    Vector arrow_pivot = vec_add(ARCHER_POSITION, (Vector){ARCHER_WIDTH/2, -ARCHER_HEIGHT/2+2});
    List* points = get_arrow_points(arrow_pivot, ARROW_WIDTH, ARROW_HEIGHT, ARROW_LENGTH);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = BULLET;
    Body* arrow = body_init_with_info(points, DEFAULT_MASS, ORANGE, type, free);
    scene_add_body(scene, arrow);
}

/** Creates an Earth-like mass to accelerate the balls */
void spawn_gravity_body(GameInfo *game_info) {
    Scene* scene = get_scene(game_info);
    // Will be offscreen, so shape is irrelevant
    List *gravity_ball = get_circle_points(VEC_ZERO, 1);
    Role *type = malloc(sizeof(*type));
    *type = NEVER_REMOVE_ON_COLLISION;
    Body *body = body_init_with_info(gravity_ball, M, BLACK, type, free);

    // Move a distance R below the scene
    Vector gravity_center = {.x = LENGTH_AND_HEIGHT.x/2, .y = -R};
    body_set_centroid(body, gravity_center);
    scene_add_body(scene, body);
}

void spawn_wall(GameInfo* game_info) {
    Scene* scene = get_scene(game_info);
    List* points = get_rectangle((Vector){-150, 0}, \
     15, 100);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = NEVER_REMOVE_ON_COLLISION;
    Body* wall = body_init_with_info(points, INFINITY, BLACK, type, free);
    scene_add_body(scene, wall);
}

Body* spawn_dart(GameInfo *game_info) {
    Scene* scene = get_scene(game_info);
    Vector offset = (Vector){45, -43};
    Vector center = vec_add(ARCHER_POSITION, offset);
    List* dart_pts = get_dart_points(center, DART_LENGTH, DART_THICKNESS);
    Role *type = malloc(sizeof(Role));
    assert(type);
    *type = PLAYER;
    Body* dart = body_init_with_info(dart_pts, DART_MASS, BLACK, type, free);

    /*
     * Now, we have to add the gravity force to the ball. We know that the
     * gravity body was instantiated after POWER_DIVISIONS bodies, followed by
     * a single arrow body.
     */
    Body* gravity_body = scene_get_body(scene, POWER_DIVISIONS + 1);
    assert(body_get_role(gravity_body) == (Role) NEVER_REMOVE_ON_COLLISION);
    create_newtonian_gravity(scene, G, gravity_body, dart);
    scene_add_body(scene, dart);
    return dart;
}

/**
 * Handles key presses from the user. Arrows keys change the direction of
 * the arrow. Power is determined by how long the space bar is held
 *
 * Each second held, the angle will change by 90 degrees.
 * @param key       the key
 * @param type      type of action (press/release)
 * @param held_time the amount of time a key is held (seconds)
 */

void on_key(char key, KeyEventType type, double held_time, void* info) {
    GameInfo* game_info = info;
    AdditionalInfo* i = get_additional_info(game_info);
    Scene* scene = get_scene(game_info);
    Body* arrow = scene_get_body(scene, POWER_DIVISIONS);
    List* points = body_get_shape(arrow);
    Vector arrow_pivot = vec_multiply(0.5, \
        vec_add(*((Vector*)list_get(points, 0)), *((Vector*)list_get(points, 1))));
    double angle = body_get_angle(arrow);
    if (type == KEY_RELEASED) {
        switch (key) {
            case UP_ARROW:
                break;
            case DOWN_ARROW:
                break;
            case RIGHT_ARROW:
                break;
            case LEFT_ARROW:
                break;
            case ' ':
            {
              if (no_darts_on_screen(scene)) {
                  double power = i->power;
                  Body* dart = spawn_dart(game_info);
                  Vector dart_velocity = (Vector){MAX_DART_VELOCITY * cos(angle), MAX_DART_VELOCITY * sin(angle)};
                  body_set_velocity(dart, vec_multiply(power, dart_velocity));
                  i->dartsLeft--;
              }
              break;
            }
        }
    }
    if (type == KEY_PRESSED) {
        switch (key) {
            case UP_ARROW:
                break;
            case DOWN_ARROW:
                break;
            case RIGHT_ARROW:
                angle -= (held_time + ARROW_HOLD_TIME) * ROTATE_ANGLE;
                body_set_rotation_custom(arrow, angle, arrow_pivot);
                break;
            case LEFT_ARROW:
                angle += (held_time + ARROW_HOLD_TIME) * ROTATE_ANGLE;
                body_set_rotation_custom(arrow, angle, arrow_pivot);
                break;
            case ' ':
            {
                if (no_darts_on_screen(scene)) {
                    double new_power;
                    double modulus_time = (held_time - ((int) held_time / (int) POWER_BAR_TIME) * POWER_BAR_TIME);
                    if (((int) held_time / (int) POWER_BAR_TIME) % 2 == 1) {
                        new_power = (POWER_BAR_TIME - modulus_time) / POWER_BAR_TIME;
                    } else {
                        new_power =  modulus_time / POWER_BAR_TIME;
                    }
                    i->power = new_power;
                    update_power_bars(game_info);
                }
                break;
            }
        }
    }
}

void spawn_balloons_l1(GameInfo* game_info) {
    Scene *scene = get_scene(game_info);
    const RGBColor RAINBOW_COLORS[7] = {RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET};
    Vector top_left = (Vector){-(NUM_COLS1 + (GAP + BALLOON_WIDTH) * NUM_COLS1) / 2, (BUFFER + NUM_ROWS1 + (GAP + BALLOON_HEIGHT) * NUM_ROWS1)/ 2};

    for (size_t i = 0; i < NUM_ROWS1; i++) {
        double y_coord = top_left.y - GAP - (BALLOON_HEIGHT + GAP) * i - BALLOON_HEIGHT / 2;
        for (size_t j = 0; j < NUM_COLS1; j++) {
            Vector balloon_center = (Vector){GAP + top_left.x + \
                (BALLOON_WIDTH + GAP) * j + BALLOON_WIDTH / 2, y_coord};
            if ((i != 0 && i != NUM_ROWS1 - 1) || (j != 0 && j != NUM_COLS1 - 1) ) {
            List* balloon_pts = get_bloon_points(balloon_center, BALLOON_WIDTH, BALLOON_HEIGHT);
            Role *type = malloc(sizeof(Role));
            assert(type);
            *type = REMOVE_ON_COLLISION;
            int color = pseudo_rand_int(0,6);
            Body* balloon = body_init_with_info(balloon_pts, INFINITY, RAINBOW_COLORS[color], type, free);
            scene_add_body(scene, balloon);
          }
        }
    }
}

void spawn_balloons_l2(GameInfo* game_info) {
    Scene *scene = get_scene(game_info);
    const RGBColor RAINBOW_COLORS[7] = {RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET};
    Vector top_left = (Vector){-(NUM_COLS2 + (GAP + BALLOON_WIDTH) * NUM_COLS2) / 2, (BUFFER + NUM_ROWS2 + (GAP + BALLOON_HEIGHT) * NUM_ROWS2)/ 2};

    for (size_t i = 0; i < NUM_ROWS2; i++) {
        double y_coord = top_left.y - GAP - (BALLOON_HEIGHT + GAP) * i - BALLOON_HEIGHT / 2;
        for (size_t j = 0; j < NUM_COLS2; j++) {
            Vector balloon_center = (Vector){GAP + top_left.x + \
                (BALLOON_WIDTH + GAP) * j + BALLOON_WIDTH / 2, y_coord};
            if (i != 3 && i != 4 && j != 3 && j != 4) {
            List* balloon_pts = get_bloon_points(balloon_center, BALLOON_WIDTH, BALLOON_HEIGHT);
            Role *type = malloc(sizeof(Role));
            assert(type);
            *type = REMOVE_ON_COLLISION;
            int color = pseudo_rand_int(0,6);
            Body* balloon = body_init_with_info(balloon_pts, INFINITY, RAINBOW_COLORS[color], type, free);
            scene_add_body(scene, balloon);
          }
        }
    }
}

void spawn_balloons_l3(GameInfo* game_info) {
    Scene *scene = get_scene(game_info);
    spawn_wall(game_info);
    const RGBColor RAINBOW_COLORS[7] = {RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET};
    Vector top_left = (Vector){-(NUM_COLS3 + (GAP + BALLOON_WIDTH) * NUM_COLS3) / 2, (BUFFER + NUM_ROWS3 + (GAP + BALLOON_HEIGHT) * NUM_ROWS3)/ 2};

    for (size_t i = 0; i < NUM_ROWS3; i++) {
        double y_coord = top_left.y - GAP - (BALLOON_HEIGHT + GAP) * i - BALLOON_HEIGHT / 2;
        for (size_t j = 0; j < NUM_COLS3; j++) {
            Vector balloon_center = (Vector){GAP + top_left.x + \
                (BALLOON_WIDTH + GAP) * j + BALLOON_WIDTH / 2, y_coord};
            if ((i != 0 && i != NUM_ROWS3 - 1) || (j != 0 && j != NUM_COLS3 - 1) ) {
            List* balloon_pts = get_bloon_points(balloon_center, BALLOON_WIDTH, BALLOON_HEIGHT);
            Role *type = malloc(sizeof(Role));
            assert(type);
            *type = REMOVE_ON_COLLISION;
            int color = pseudo_rand_int(0,6);
            Body* balloon = body_init_with_info(balloon_pts, INFINITY, RAINBOW_COLORS[color], type, free);
            scene_add_body(scene, balloon);
          }
        }
    }
}

void spawn_sprite(GameInfo* game_info, char* filename, Vector position, int width,
    int height) {
    Sprite *sprite = sprite_init(position, filename, width, height);
    list_add(get_sprites(game_info), sprite);
}

void spawn_text(GameInfo* game_info, char* t, Vector position, RGBColor color, \
    char* font_type, int font_size, int width, int height) {
    Text *tex = text_init(position, width, height, t, font_size, font_type, color);
    list_add(get_text(game_info), tex);
}

GameInfo* setup_game(void) {
    initialize_window(LENGTH_AND_HEIGHT);
    Scene* scene = initialize_scene();
    AdditionalInfo* info = malloc(sizeof(AdditionalInfo));
    assert(info);
    info->power = 0;
    info->level = 1;
    info->dartsLeft = 5;
    info->target = 15;
    GameInfo* game_info = game_info_init(scene, info, free_additional_info);
    return game_info;
}

void clear_balloons(GameInfo* game_info) {
    Scene *scene = get_scene(game_info);

    for (size_t i = BALLOON_IDX_START-1; i < scene_bodies(scene); i++) {
        Body* body = scene_get_body(scene, i);

        if (body_get_role(body) == REMOVE_ON_COLLISION) {
            body_remove(body);
        }
    }
}

int balloons_left(GameInfo* game_info) {
    Scene *scene = get_scene(game_info);
    size_t count = 0;

    for (size_t i = BALLOON_IDX_START; i < scene_bodies(scene); i++) {
        Body* body = scene_get_body(scene, i);

        if (body_get_role(body) == REMOVE_ON_COLLISION) {
            count++;
        }
    }

    return count;
}

void destroy_balloons(Scene *scene, Body* dart, GameInfo* game_info) {
    int left = balloons_left(game_info);

    for (size_t i = BALLOON_IDX_START; i < BALLOON_IDX_START+left+1; i++) {
        Body *curr_body = scene_get_body(scene, i);
        create_destructive_collision(scene, curr_body, dart);
    }
}

int restart(GameInfo* game_info) {
    AdditionalInfo* info = get_additional_info(game_info);
    Scene *scene = get_scene(game_info);

    if ((info->dartsLeft == 0) && (balloons_left(game_info) > info->target) && (no_darts_on_screen(scene)))  {
        return 1;
    } else {
        return 0;
    }
}

int level_completed(GameInfo* game_info) {
    Scene *scene = get_scene(game_info);
    AdditionalInfo* info = get_additional_info(game_info);

    if ((info->dartsLeft == 0) && (balloons_left(game_info) <= info->target) && (no_darts_on_screen(scene))) {
        return 1;
    } else {
        return 0;
    }
}

void destroy_bullet(Scene *scene) {
    for (size_t i = BALLOON_IDX_START - 3; i < scene_bodies(scene); i++) {
        Body *body = scene_get_body(scene, i);
        if (body_get_role(body) == PLAYER) {
            if (body_get_centroid(body).y + DART_LENGTH < (LENGTH_AND_HEIGHT.y * -0.5) || fabs(body_get_centroid(body).x + DART_LENGTH) > (LENGTH_AND_HEIGHT.x * 0.5)) {
                body_remove(body);
            }
        }
    }
}

void load_level(GameInfo* game_info) {
    AdditionalInfo* info = get_additional_info(game_info);
    info->dartsLeft = 5;
    clear_balloons(game_info);

    if (info->level == 1) {
        spawn_balloons_l1(game_info);
    } else if (info->level == 2) {
        spawn_balloons_l2(game_info);
    } else {
        spawn_balloons_l3(game_info);
    }
}


int main(int argc, char* argv[]) {

    GameInfo* game_info = setup_game();
    double dt;
    double time_elapsed = 0;
    /* The ordering of initialization is very important because we use the
     * ordering to determine which index a certain body of the scene is at. */
    initialize_power_bars(game_info);
    spawn_arrow(game_info);
    spawn_gravity_body(game_info);
    load_level(game_info);
    spawn_sprite(game_info, PLAYER_SPRITE_PATH, ARCHER_POSITION, ARCHER_WIDTH, \
        ARCHER_HEIGHT);

    // NOTE: In the future, when we actually have a score, we will have to
    // malloc a char * to display the text "Score: [score]" <-- We did this 
    // but for some reason it was only working on windows and not mac. On mac
    // the score would display for a second and then crash on a "Cannot open font"
    // error
    spawn_text(game_info, "You have 5 darts to have 15 balloons left to pass", SCORE_TEXT_POSITION, BLACK, FONT_TYPE, FONT_SIZE, \
        SCORE_TEXT_WIDTH, SCORE_TEXT_HEIGHT);

    Scene *scene = get_scene(game_info);
    sdl_on_key(on_key, game_info);

    while (!sdl_is_done()) {
        dt = time_since_last_tick();
        time_elapsed += dt;
        // Multiply by constant to speed up physics
        scene_tick(scene, 3 * dt);
        scene_tick_no_forces(scene, 3 * dt);

        if (!no_darts_on_screen(scene)) {
            Body* dart = scene_get_body(scene, scene_bodies(scene) - 1);
            destroy_balloons(scene, dart, game_info);
            destroy_bullet(scene);
        }

        sdl_render_game(game_info);

        if (restart(game_info)) {
            load_level(game_info);
        }

        if (level_completed(game_info)) {
            AdditionalInfo* info = get_additional_info(game_info);

            if (info->level == 3) {
                printf("Congratulations you win!\n");
                game_info_free(game_info);
                exit(0);
            } else {
                info->level++;
                load_level(game_info);
            }
        }
    }
    game_info_free(game_info);
    return 0;
}
