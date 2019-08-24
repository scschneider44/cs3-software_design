#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "body.h"
#include "test_util.h"
#include <stdio.h>

struct body {
    List *points;
    Vector *velocity;
    Vector *acceleration;
    Vector *centroid;
    Vector *elasticity;
    Vector *forces;
    Vector *impulses;
    void *info;
    FreeFunc info_freer;
    RGBColor color;
    double mass;
    double angle;
    Body* other;
    double time_since_last_collision;
};

struct bodyInfo {
    void* info;
    int existence;
};

Body *body_init(List *shape, double mass, RGBColor color) {
    return body_init_with_info(shape, mass, color, NULL, free);
}

BodyInfo* body_info_init(void* info) {
    BodyInfo* b_i = malloc(sizeof(BodyInfo));
    assert(b_i);
    b_i->info = info;
    b_i->existence = NOT_REMOVED;
    return b_i;
}

Body *body_init_with_info(
    List *shape, double mass, RGBColor color, void *info, FreeFunc info_freer
) {
    assert(mass > 0);
    Body *body = malloc(sizeof(Body));
    assert(body);
    body->points = shape;
    body->velocity = create_vector_p(VEC_ZERO);
    body->acceleration = create_vector_p(VEC_ZERO);
    body->elasticity = create_vector_p(VEC_ZERO);
    body->centroid = create_vector_p(body_calculate_centroid(body));
    body->forces = create_vector_p(VEC_ZERO);
    body->impulses = create_vector_p(VEC_ZERO);
    body->other = NULL;
    BodyInfo* b_i = body_info_init(info);
    body->info = b_i;
    body->info_freer = info_freer;
    body->color = color;
    body->mass = mass;
    body->angle = 0;
    body->time_since_last_collision = 1;
    return body;
}

void body_free(void *b) {
    assert(b);
    Body* body = b;
    list_free(body->points);
    vector_free(body->velocity);
    vector_free(body->acceleration);
    vector_free(body->forces);
    vector_free(body->impulses);
    vector_free(body->elasticity);
    vector_free(body->centroid);
    BodyInfo* i = body->info;
    body->info_freer(i->info);
    free(i);
    free(body);
}

List *body_get_shape(Body *body) {
    assert(body);
    return body->points;
}

void *body_get_info(Body *body) {
    assert(body);
    return ((BodyInfo*)(body->info))->info;
}

double body_get_angle(Body *body) {
    assert(body);
    return body->angle;
}

Vector body_get_elasticity(Body *body) {
    assert(body);
    return *body->elasticity;
}

Vector body_get_centroid(Body *body) {
    assert(body);
    return *body->centroid;
}

Vector body_get_velocity(Body *body) {
    assert(body);
    return *body->velocity;
}

Vector body_get_acceleration(Body *body) {
    assert(body);
    return *body->acceleration;
}

double body_get_mass(Body *body) {
    assert(body);
    return body->mass;
}

RGBColor body_get_color(Body *body) {
    assert(body);
    return body->color;
}

Role body_get_role(Body *body) {
    assert(body);
    return *((Role*)(((BodyInfo*)(body->info))->info));
}

double body_get_time_since_last_collision(Body *body) {
  assert(body);
  return body->time_since_last_collision;
}

void body_set_centroid(Body *body, Vector new_centroid) {
    assert(body);
    Vector diff = vec_subtract(new_centroid, *(body->centroid));

    *(body->centroid) = new_centroid;

    for (size_t i = 0; i < list_size(body->points); i++) {
        list_set(body_get_shape(body), i, create_vector_p(vec_add(diff, \
            *(Vector *)list_get(body_get_shape(body), i))));
    }
}

void body_set_elasticity(Body *body, Vector v) {
    assert(body);
    *(body->elasticity) = v;
}

void body_set_acceleration(Body *body, Vector v) {
    assert(body);
    *(body->acceleration) = v;
}

void body_set_velocity(Body *body, Vector v) {
    assert(body);
    *(body->velocity) = v;
}

void body_set_rotation_custom(Body *body, double angle, Vector pivot) {
    assert(body);
    double diff = angle - body->angle;
    if (diff == 0) {
        return;
    }
    List *points_list = body_get_shape(body);
    for (size_t i = 0; i< list_size(points_list); i++) {
        Vector *v_i = list_get(points_list, i);
        /**
            * we first move the vector back to the origin, rotate
            * and then move the vector back to where it was to get respect
            * to the centroid to rotate about
            */
        Vector v_i_origin = vec_subtract(*v_i, pivot);
        Vector v_i_origin_rotate = vec_rotate(v_i_origin, diff);
        Vector rotated_vec = vec_add(v_i_origin_rotate, pivot);
        list_set(points_list, i, create_vector_p(rotated_vec));
    }
    body->angle = angle;
}

void body_set_rotation(Body *body, double angle) {
    body_set_rotation_custom(body, angle, body_get_centroid(body));
}

void body_set_force(Body *body, Vector force) {
    assert(body);
    *(body->forces) = force;
}

void body_set_impulse(Body *body, Vector impulse) {
    assert(body);
    *(body->impulses)= impulse;
}

Body* body_get_colliding_body(Body *body) {
    assert(body);
    return body->other;
}

void body_set_colliding_body(Body *body, Body* other) {
    assert(body);
    body->other = other;
}

void body_set_role(Body *body, Role role) {
  assert(body);
  *((Role*)(((BodyInfo*)(body->info))->info)) = role;
}

void body_set_color(Body *body, RGBColor color) {
  assert(body);
  body->color = color;
}


void body_set_angle(Body *body, double angle) {
  assert(body);
  body->angle = angle;
}

void body_set_time_since_last_collision(Body *body, double time) {
  assert(body);
  body->time_since_last_collision = time;
}

void body_rotate_with_velocity(Body *body) {
    // Rotate body to be in alignment with its velocity
    Vector current_vel = *(body->velocity);
    if (current_vel.x == 0 && current_vel.y == 0) return;
    body_set_rotation(body, vec_angle(current_vel));
}

void body_tick(Body *body, double dt) {
    assert(body);

    // If mass is infinity, it should not move
    if (body->mass == INFINITY) {
        return;
    }
    Vector* start_velocity = body->velocity;
    // J = F*t = mv_2 - mv_1
    Vector total_impulses = vec_add(*(body->impulses), \
        vec_multiply(dt, *(body->forces)));

    Vector velocity_change = vec_multiply(1 / body->mass, total_impulses);
    Vector end_velocity = vec_add(*start_velocity, velocity_change);

    // Newton's second law. F = ma --> a = F/m
    *(body->acceleration) = vec_multiply(1 / body->mass, *(body->forces));

    // d = v_(avg) * t
    Vector translate = vec_multiply(dt, vec_multiply(0.5, \
        vec_add(*start_velocity, end_velocity)));
    body_translate(body, translate);

    body_set_velocity(body, end_velocity);

    // Reset forces and impulses
    body_set_force(body, VEC_ZERO);
    body_set_impulse(body, VEC_ZERO);
    body_rotate_with_velocity(body);
}

void body_tick_no_forces(Body *body, double dt) {
    assert(body);
    // d = vt + at^2/2
    Vector translate = vec_add(vec_multiply(dt, *(body->velocity)),
        vec_multiply(dt * dt * 0.5, *(body->acceleration)));
    body_translate(body, translate);

    // v_f = v_i + at
    body_set_velocity(body, (vec_add(*(body->velocity), \
        vec_multiply(dt, *(body->acceleration)))));
    body_rotate_with_velocity(body);

}


void body_add_force(Body *body, Vector force) {
    assert(body);
    body->forces->x += force.x;
    body->forces->y += force.y;
}

void body_add_impulse(Body *body, Vector impulse) {
    assert(body);
    body->impulses->x += impulse.x;
    body->impulses->y += impulse.y;
}

double body_area(Body* body) {
    assert(body);
    List* points_list = body->points;
    size_t n = list_size(points_list);
    Vector *v_n = list_get(points_list, n-1);
    Vector *v_1 = list_get(points_list, 0);
    double shoelace_sum = vec_cross(*v_n, *v_1);

    /* Used first formula on wiki with 2 summations and 2 other terms. */
    for (size_t i = 0; i <= n-2; i++) {
        Vector *v_i = list_get(points_list, i);
        Vector *v_i_plus_one = list_get(points_list, i+1);

        shoelace_sum += vec_cross(*v_i, *v_i_plus_one);
    }

    return .5 * fabs(shoelace_sum);
}

Vector body_calculate_centroid(Body* body) {
    assert(body);
    List* points_list = body->points;
    double area = body_area(body);
    double c_x = 0;
    double c_y = 0;

    for (size_t i = 0; i < list_size(points_list); i++) {
        /**
            * we need to prepare for the event that we reach the end of the
            * vector_list and we need the first vector for our i+1 term. To prevent
            * an out of bounds error, we will just set v_i_plus_one to the first
            * element and if we are not at the last element, we can just set the i+1
            * term as usual. Otherwise, we use the 0th Vector for our i+1
            */
        Vector *v_i_plus_one = list_get(points_list, 0);

        if (i != list_size(points_list)-1) {
            v_i_plus_one = list_get(points_list, i+1);
        }

        Vector *v_i = list_get(points_list, i);
        double common_term_in_sum = vec_cross(*v_i, *v_i_plus_one);

        c_x += (v_i->x + v_i_plus_one->x) * common_term_in_sum;
        c_y += (v_i->y + v_i_plus_one->y) * common_term_in_sum;
    }

    c_x /= (6 * area);
    c_y /= (6 * area);

    return vec_init(c_x, c_y);
}

void body_translate(Body *body, Vector translation) {
    assert(body);
    Vector current_centroid = body_get_centroid(body);
    Vector new_centroid = vec_add(current_centroid, translation);
    body_set_centroid(body, new_centroid);
}

void body_remove(Body *body) {
    assert(body);
    BodyInfo *info = body->info;
    info->existence = REMOVED;
}

bool body_is_removed(Body *body) {
    assert(body);
    BodyInfo *info = body->info;
    if (info) {
        return info->existence == REMOVED;
    }
    return false;
}
