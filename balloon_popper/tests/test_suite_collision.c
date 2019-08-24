#include "vector.h"
#include "list.h"
#include "collision.h"
#include "test_util.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>



// Make square at (+/-1, +/-1)
List *make_square1() {
    List *sq = list_init(4, vector_free);
    list_add(sq, create_vector_p((Vector){1,  1}));
    list_add(sq, create_vector_p((Vector){-1, 1}));
    list_add(sq, create_vector_p((Vector){-1, -1}));
    list_add(sq, create_vector_p((Vector){1,  -1}));
    return sq;
}

// Make square at (+/-1, +/-1)
List *make_square2() {
    List *sq = list_init(4, vector_free);
    list_add(sq, create_vector_p((Vector){2,  2}));
    list_add(sq, create_vector_p((Vector){-2, 2}));
    list_add(sq, create_vector_p((Vector){-2, -2}));
    list_add(sq, create_vector_p((Vector){2,  -2}));
    return sq;
}

List *make_oval() {
  return get_oval_points((Vector){0, 0}, 40, 40);
}
List *make_oval2() {
  return get_oval_points((Vector){0, 0}, 20, 20);
}
List *make_oval3() {
  return get_oval_points((Vector){20, 20}, 80, 80);
}

List *make_invader() {
  return get_partial_circle(30, 0, 10, (Vector){0, 0});
}


// Make 3-4-5 triangle
List *make_triangle() {
    List *tri = list_init(3, vector_free);
    list_add(tri, create_vector_p((Vector){0, 0}));
    list_add(tri, create_vector_p((Vector){4, 0}));
    list_add(tri, create_vector_p((Vector){4, 3}));
    return tri;
}

List *make_triangle_perf() {
    List *tri_perf = list_init(3, vector_free);
    list_add(tri_perf, create_vector_p((Vector){0,  3}));
    list_add(tri_perf, create_vector_p((Vector){-3, 0}));
    list_add(tri_perf, create_vector_p((Vector){3, 0}));
    return tri_perf;
}


// Make square at (+/-1, +/-1)
List *make_pent() {
    List *pent = list_init(5, vector_free);
    list_add(pent, create_vector_p((Vector){0,  3}));
    list_add(pent, create_vector_p((Vector){-3, 0}));
    list_add(pent, create_vector_p((Vector){-2,  -2}));
    list_add(pent, create_vector_p((Vector){2,  -2}));
    list_add(pent, create_vector_p((Vector){3, 0}));
    return pent;
}

void test_find_collision() {
  List *tri = make_triangle();
  List *tri_perf = make_triangle_perf();
  List *sq1 = make_square1();
  List *sq2 = make_square2();
  List *pent = make_pent();
  List *oval = make_oval();
  List *oval2 = make_oval2();
  List *oval3 = make_oval3();
  List *invader = make_invader();
  assert(find_collision(sq1, sq2).y != 0);

  //assert(find_collision(oval, oval2) == true);

  assert(find_collision(oval, oval3).x != 0);
  printf("END OF FIRST\n");
  assert(find_collision(tri, sq1).y != 0);

  //assert(find_collision(pent, tri) == true);
  //assert(find_collision(pent, pent) == true);
  list_free(tri);
  list_free(tri_perf);
  list_free(sq1);
  list_free(sq2);
  list_free(pent);
  list_free(oval);
  list_free(oval2);
  list_free(oval3);
  list_free(invader);
}


int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_find_collision);

    puts("NICE PASS");

    return 0;
}
