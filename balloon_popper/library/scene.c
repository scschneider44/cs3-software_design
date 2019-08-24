#include "scene.h"
#include "body.h"
#include "list.h"
#include "forces.h"
#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define NUMBER_STARTING_BODIES 5

// A scene is simply a list of bodies and force creators.
struct scene {
    List* bodies;
    List* forceInfos;
};

struct forceInfo {
    ForceCreator forcer;
    void *aux;
    FreeFunc aux_freer;
    List* bodies;
};

Scene *scene_init(void) {
    Scene* scene = malloc(sizeof(Scene));
    assert(scene);
    scene->bodies = list_init(NUMBER_STARTING_BODIES, body_free);
    scene->forceInfos = list_init(0, forceInfo_free);
    return scene;
}

void forceInfo_free(void *force) {
    ForceInfo* f = force;
    // f->bodies is same as f->aux->bodies so we do not free here
    if (f->aux_freer) {
        f->aux_freer(f->aux);
    }
    free(f);
}

void scene_free(Scene *scene) {
    assert(scene);
    list_free(scene->bodies);
    list_free(scene->forceInfos);
    free(scene);
}

size_t scene_bodies(Scene *scene) {
    assert(scene);
    return list_size(scene->bodies);
}

size_t scene_forces(Scene *scene) {
    assert(scene);
    return list_size(scene->forceInfos);
}

Body *scene_get_body(Scene *scene, size_t index) {
    assert(scene);
    assert(0 <= index && index < scene_bodies(scene));
    return list_get(scene->bodies, index);
}

ForceInfo* scene_get_forces(Scene* scene, size_t index) {
    assert(scene);
    assert(0 <= index && index < scene_forces(scene));
    return list_get(scene->forceInfos, index);
}

void scene_add_body(Scene *scene, Body *body) {
    assert(scene);
    assert(body);
    list_add(scene->bodies, body);
}

void scene_remove_body(Scene *scene, size_t index) {
    assert(scene);
    list_remove(scene->bodies, index);
}

void scene_tick(Scene *scene, double dt) {
    assert(scene);

    // Step 1: Iterate through all forces and apply
    size_t i;
    size_t k;
    for (i = 0; i < scene_forces(scene); i++) {
        ForceInfo* force = scene_get_forces(scene, i);
        force->forcer(force->aux);
    }
    i = 0;
    k = scene_forces(scene);

    bool to_remove;
    // Step 2: Remove forces that have had one of its bodies removed
    while (i < k) {
        to_remove = false;
        ForceInfo* force = scene_get_forces(scene, i);
        List *bodies = force->bodies;
        for (size_t j = 0; j < list_size(bodies); j++) {
            Body* b = list_get(bodies, j);
            if (body_is_removed(b)) {
                list_remove(scene->forceInfos, i);
                k = scene_forces(scene);
                to_remove = true;
                break;
            }
        }
        if (!to_remove) {
            i++;
        }
    }
    // Step 3: Removes all bodies that are marked to be removed
    i = 0;
    k = scene_bodies(scene);
    while (i < k) {
        Body *b = scene_get_body(scene, i);
        if (body_is_removed(b)) {
            // Vector center = body_get_centroid(scene_get_body(scene, i));
            scene_remove_body(scene, i);
            // List* explosion_points = get_star_points(8, 15, center);
            // Role *type = malloc(sizeof(Role));
            // assert(type);
            // *type = PLAYER;
            // Body* explosion = body_init_with_info(explosion_points, 1, (RGBColor) {1, 1, 1}, type, free);
            // scene_add_body(scene, explosion);
            // scene_remove_body(scene, i);
        } else {
            body_tick(b, dt);
            i++;
        }
        k = scene_bodies(scene);
    }
}

void scene_tick_no_forces(Scene *scene, double dt) {
    assert(scene);
    // Iterate over bodies
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        Body *b = scene_get_body(scene, i);
        body_tick_no_forces(b, dt);
    }
}

void scene_add_special_body(
    Scene* scene,
    RGBColor color,
    List *points,
    double mass,
    Vector start_vel,
    Vector start_acc,
    Vector elasticity
) {
    assert(scene);

    if (mass < 0) {
        mass = DEFAULT_MASS;
    }

    Body *special_body = body_init(points, mass, color);
    body_set_velocity(special_body, start_vel);
    body_set_acceleration(special_body, start_acc);
    body_set_elasticity(special_body, elasticity);
    scene_add_body(scene, special_body);
}

void scene_add_force_creator(
    Scene *scene,
    ForceCreator forcer,
    void *aux,
    FreeFunc freer
) {
    scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_add_bodies_force_creator(
    Scene *scene, ForceCreator forcer, void *aux, List *bodies, FreeFunc freer
) {
    assert(scene);
    ForceInfo* force_info = malloc(sizeof(ForceInfo));
    assert(force_info);
    force_info->forcer = forcer;
    force_info->aux = aux;
    force_info->aux_freer = freer;
    force_info->bodies = bodies;
    list_add(scene->forceInfos, force_info);
}
