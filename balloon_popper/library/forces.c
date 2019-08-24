#include "forces.h"
#include "utils.h"
#include "list.h"
#include "collision.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
struct forceAux {
    List* bodies;
    double constant;    // G or K or gamma
};

struct collisionAux {
    List* bodies;
    CollisionHandler handler;
    void* info;
};

struct elas {
    double elasticity;
};

void handleDestructiveCollision(Body *body1, Body *body2, Vector axis, void *aux) {
    if (body_get_role(body2) == PLAYER) {
      if (body_get_role(body1) == REMOVE_ON_COLLISION) {
          body_remove(body1);
      }

      else if (body_get_role(body1) == NEVER_REMOVE_ON_COLLISION) {
          body_remove(body2);
      }
    }

    else {
        if (body_get_role(body1) != (Role) NEVER_REMOVE_ON_COLLISION) {
            body_remove(body1);
        }
        if (body_get_role(body2) != (Role) NEVER_REMOVE_ON_COLLISION) {
            body_remove(body2);
        }
    }
}

void handlePhysicsCollision(Body *body1, Body *body2, Vector axis, void *aux) {
    // Axix points from body1 -> body2
    Elas* a = aux;
    double mass_1 = body_get_mass(body1);
    double mass_2 = body_get_mass(body2);
    Vector vel_1 = body_get_velocity(body1);
    Vector vel_2 = body_get_velocity(body2);
    double elas = a->elasticity;
    Vector component_1 = vec_projection(vel_1, axis);
    Vector component_2 = vec_projection(vel_2, axis);
    double reduced_mass;
    if (mass_1 == INFINITY) {
        reduced_mass = mass_2;
    } else if (mass_2 == INFINITY) {
        reduced_mass = mass_1;
    } else {
        reduced_mass = mass_1 * mass_2 / (mass_1 + mass_2);
    }
    double cons = reduced_mass * (1 + elas);

    Vector impulse = vec_multiply(cons, vec_subtract(component_2, component_1));

    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_multiply(-1, impulse));

    if (body_get_role(body1) == (Role) REMOVE_ON_COLLISION) {
      body_remove(body1);
    }
    if (body_get_role(body2) == (Role) REMOVE_ON_COLLISION) {
      body_remove(body2);
    }
    if (body_get_role(body1) == (Role) TURN_WHITE_ON_COLLISION) {
      body_set_color(body1, (RGBColor) {1, 1, 1});
      body_set_role(body1, (Role) REMOVE_ON_COLLISION);
    }
    if (body_get_role(body2) == (Role) TURN_WHITE_ON_COLLISION) {
      body_set_color(body2, (RGBColor) {1, 1, 1});
      body_set_role(body2, (Role) REMOVE_ON_COLLISION);
    }

}

void applyDirectionalForce(Body* b1, Body* b2, double magnitude_force) {
    Vector direction_b1_b2 = vec_unit_vector(vec_subtract(body_get_centroid(b2), \
        body_get_centroid(b1)));
    Vector direction_b2_b1 = vec_multiply(-1, direction_b1_b2);
    /* direction_b1_b2 and direction_b2_b1 are both unit vectors. Because
     * these forces are attractive, we need to apply the force in a manner so
     * that b1 and b2 become closer together.
     */
    body_add_force(b1, vec_multiply(magnitude_force, direction_b1_b2));
    body_add_force(b2, vec_multiply(magnitude_force, direction_b2_b1));
}

void addGravityForce(void *aux) {
    ForceAux* a = aux;
    Body* b1 = list_get(a->bodies, 0);
    Body* b2 = list_get(a->bodies, 1);
    // Don't apply gravity force if the two bodies are too close
    if (is_too_close(b1, b2)) {
        return;
    }
    double distance = vec_distance(body_get_centroid(b1), body_get_centroid(b2));
    // F = G * m1 * m2 / r^2
    double mag_force = a->constant * body_get_mass(b1) * body_get_mass(b2) / \
        (distance * distance);
    applyDirectionalForce(b1, b2, mag_force);
}

void addSpringForce(void *aux) {
    ForceAux* a = aux;
    Body* b1 = list_get(a->bodies, 0);
    Body* b2 = list_get(a->bodies, 1);
    double distance = vec_distance(body_get_centroid(b1), body_get_centroid(b2));
    // F = -kx
    double mag_force = a->constant * distance;
    applyDirectionalForce(b1, b2, mag_force);
    return;
}

void addDragForce(void *aux) {
    ForceAux* a = aux;
    Body* body = list_get(a->bodies, 0);
    // Drag should be proportional to velocity, and in the opposite direction
    Vector drag_force = vec_multiply(-(a->constant), body_get_velocity(body));
    body_add_force(body, drag_force);
}

void addCollision(void *aux) {
    // Should check for collision
    CollisionAux* a = aux;
    Body* b1 = list_get(a->bodies, 0);
    Body* b2 = list_get(a->bodies, 1);
    Vector collision = find_collision(body_get_shape(b1), body_get_shape(b2));
    if (collision.x != 0 || collision.y != 0) {
        // If bodies are both collided previously, then do not apply again
        // if (body_get_colliding_body(b1) == b2 && body_get_colliding_body(b2) == b1) {
        //     return;
        // }
        if (body_get_time_since_last_collision(b1) < 0.01 && body_get_role(b1) == BULLET) {
          return;
        }
        body_set_colliding_body(b1, b2);
        body_set_colliding_body(b2, b1);
        body_set_time_since_last_collision(b1, 0);
        a->handler(b1, b2, collision, a->info);
    } else {
        body_set_colliding_body(b1, NULL);
        body_set_colliding_body(b2, NULL);
    }
}

void create_newtonian_gravity(Scene *scene, double G, Body *body1, Body *body2)
{
    ForceAux* aux = malloc(sizeof(ForceAux));
    assert(aux);
    aux->constant = G;
    aux->bodies = list_init(2, body_free);
    list_add(aux->bodies, body1);
    list_add(aux->bodies, body2);
    scene_add_bodies_force_creator(scene, addGravityForce, aux, \
        aux->bodies, aux_freer);
}

void create_spring(Scene *scene, double k, Body *body1, Body *body2) {
    ForceAux* aux = malloc(sizeof(ForceAux));
    assert(aux);
    aux->constant = k;
    aux->bodies = list_init(2, body_free);
    list_add(aux->bodies, body1);
    list_add(aux->bodies, body2);
    scene_add_bodies_force_creator(scene, addSpringForce, aux, \
        aux->bodies, aux_freer);
}

void create_drag(Scene *scene, double gamma, Body *body) {
    ForceAux* aux = malloc(sizeof(ForceAux));
    assert(aux);
    aux->constant = gamma;
    aux->bodies = list_init(1, body_free);
    list_add(aux->bodies, body);
    scene_add_bodies_force_creator(scene, addDragForce, aux, \
        aux->bodies, aux_freer);
}

void create_collision(
    Scene *scene,
    Body *body1,
    Body *body2,
    CollisionHandler handler,
    void *aux,
    FreeFunc freer
) {
    CollisionAux* c_aux = malloc(sizeof(CollisionAux));
    assert(c_aux);
    c_aux->handler = handler;
    c_aux->info = aux;
    c_aux->bodies = list_init(2, body_free);
    list_add(c_aux->bodies, body1);
    list_add(c_aux->bodies, body2);
    scene_add_bodies_force_creator(scene, addCollision, c_aux, \
        c_aux->bodies, freer);
}

void create_physics_collision(
    Scene *scene, double elasticity, Body *body1, Body *body2
) {
    Elas *e = malloc(sizeof(Elas));
    assert(e);
    e->elasticity = elasticity;
    create_collision(scene, body1, body2, handlePhysicsCollision, e, aux_freer);
}

void create_destructive_collision(Scene *scene, Body *body1, Body *body2) {
    CollisionAux* aux = malloc(sizeof(CollisionAux));
    assert(aux);
    aux->handler = handleDestructiveCollision;
    aux->bodies = list_init(2, body_free);
    list_add(aux->bodies, body1);
    list_add(aux->bodies, body2);
    create_collision(scene, body1, body2, handleDestructiveCollision, aux, aux_freer);
}

void aux_freer(void *a) {
    ForceAux *aux = a;
    free(aux->bodies);
    free(aux);
}
