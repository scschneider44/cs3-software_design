#include "forces.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

List *make_shape() {
    List *shape = list_init(4, free);
    Vector *v = malloc(sizeof(*v));
    *v = (Vector) {-1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (Vector) {+1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (Vector) {+1, +1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (Vector) {-1, +1};
    list_add(shape, v);
    return shape;
}

double gravity_potential(double G, Body *body1, Body *body2) {
    Vector r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    return -G * body_get_mass(body1) * body_get_mass(body2) / sqrt(vec_dot(r, r));
}

double kinetic_energy(Body *body) {
    Vector v = body_get_velocity(body);
    return body_get_mass(body) * vec_dot(v, v) / 2;
}

void test_energy_conservation_with_drag() {
    const double M1 = 4.5, M2 = 7.3;
    const double G = 1e3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    const double TAU = .4;
    Scene *scene = scene_init();
    Body *mass1 = body_init(make_shape(), M1, (RGBColor) {0, 0, 0});
    scene_add_body(scene, mass1);
    Body *mass2 = body_init(make_shape(), M2, (RGBColor) {0, 0, 0});
    body_set_centroid(mass2, (Vector) {10, 20});
    scene_add_body(scene, mass2);
    create_newtonian_gravity(scene, G, mass1, mass2);
    create_drag(scene, TAU, mass1);
    double last_energy = gravity_potential(G, mass1, mass2);
    Vector last_centroid = body_get_centroid(mass1);
    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
        Vector current_centroid = body_get_centroid(mass1);
        double work = vec_magnitude(body_get_velocity(mass1)) * TAU * vec_distance(current_centroid, last_centroid);
        double current_energy = gravity_potential(G, mass1, mass2) +
          kinetic_energy(mass1) + kinetic_energy(mass2);
        assert(within(1e-4, last_energy-current_energy, work));

        last_energy = current_energy;
        last_centroid = current_centroid;
    }
    scene_free(scene);
}

void test_newtonian_gravity() {
    const double M1 = 4.5, M2 = 7.3;
    const double DT = 1e-6;
    const double G = 1e3;
    const int STEPS1 = 1000000;
    const int STEPS2 = 1500000;
    const int STEPS3 = 2000000;
    Scene *scene = scene_init();
    Body *mass1 = body_init(make_shape(), M1, (RGBColor) {0, 0, 0});
    body_set_velocity(mass1, VEC_ZERO);
    scene_add_body(scene, mass1);

    Body *mass2 = body_init(make_shape(), M2, (RGBColor) {0, 0, 0});
    body_set_centroid(mass2, (Vector) {10, 20});
    body_set_velocity(mass2, VEC_ZERO);
    scene_add_body(scene, mass2);
    double last_distance = vec_distance(body_get_centroid(mass1), body_get_centroid(mass2));
    create_newtonian_gravity(scene, G, mass1, mass2);
    size_t i = 0;

    for (i = 0; i < STEPS1; i++) {
        scene_tick(scene, DT);
        double current_distance = vec_distance(body_get_centroid(mass1), body_get_centroid(mass2));
        assert(current_distance < last_distance);
        last_distance = current_distance;
    }

    while (++i < STEPS2) {
        scene_tick(scene, DT);
    }

    last_distance = vec_distance(body_get_centroid(mass1), body_get_centroid(mass2));
    while (++i < STEPS3) {
        scene_tick(scene, DT);
        double current_distance = vec_distance(body_get_centroid(mass1), body_get_centroid(mass2));
        assert(current_distance > last_distance);
        last_distance = current_distance;
    }

    scene_free(scene);
}

void test_drag() {
    const double M = 10;
    const double A = 3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    Scene *scene = scene_init();
    Body *mass = body_init(make_shape(), M, (RGBColor) {0, 0, 0});
    body_set_centroid(mass, (Vector) {A, 0});
    body_set_velocity(mass, (Vector) {10, 50});
    scene_add_body(scene, mass);
    double last_energy = kinetic_energy(mass);
    create_drag(scene, .4, mass);
    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
        double current_energy = kinetic_energy(mass);
        assert(current_energy < last_energy);
        last_energy = current_energy;
    }

    scene_free(scene);
}

void test_zero_drag_no_slow_down() {
    const double M = 10;
    const double A = 3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    Scene *scene = scene_init();
    Body *mass = body_init(make_shape(), M, (RGBColor) {0, 0, 0});
    body_set_centroid(mass, (Vector) {A, 0});
    body_set_velocity(mass, (Vector) {10, 50});
    scene_add_body(scene, mass);
    double last_energy = kinetic_energy(mass);
    
    create_drag(scene, 0, mass);
    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
        double current_energy = kinetic_energy(mass);
        assert(current_energy == last_energy);
        last_energy = current_energy;
    }

    scene_free(scene);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
      read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_energy_conservation_with_drag)
    DO_TEST(test_newtonian_gravity)
    DO_TEST(test_drag)
    DO_TEST(test_zero_drag_no_slow_down)

    puts("forces_test PASS");
    return 0;
}
