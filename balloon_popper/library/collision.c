#include "collision.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

Vector find_collision(List *shape1, List *shape2) {
  Vector collision_axis;
  double *overlap1 = (double *)malloc(sizeof(double));
  double *overlap2 = (double *)malloc(sizeof(double));
  Vector axis1 = check_shape_axes(shape1, shape2, overlap1);
  Vector axis2 = check_shape_axes(shape2, shape1, overlap2);
  if (overlap1 < overlap2) {
    collision_axis = axis1;
  }
  collision_axis = axis2;
  free(overlap1);
  free(overlap2);
  return collision_axis;
}

Vector check_shape_axes(List *shape1, List *shape2, double *min_overlap) {
  Vector min_overlap_axis = VEC_ZERO;
  *min_overlap = 100000000;
  size_t length = list_size(shape1);
  /*
   * j is the last index of shape so we will start with the edge between the
   * first and last vertices.
   */
  size_t j = list_size(shape1) - 1;
  for (size_t i = 0; i < length; i++) {
    Vector p_line = get_projection_line(list_get(shape1, i), list_get(shape1, j));
    double overlap_size = overlap(shape1, shape2, p_line);
    if(overlap_size == 0) {
      *min_overlap = 100000000;
      return VEC_ZERO;
    }
    if (overlap_size < *min_overlap) {
      *min_overlap = overlap_size;
      min_overlap_axis = vec_subtract(*(Vector *)list_get(shape1, i), *(Vector *)list_get(shape1, j));
    }
    j = i;
  }
  return min_overlap_axis;
}

Vector get_projection_line(Vector *point1, Vector *point2) {
  /* subtracting two vectors gets the edge between them */
  Vector axis = vec_subtract(*point1, *point2);
  return vec_unit_vector(axis);
}

double overlap(List *shape1, List *shape2, Vector projection_line) {
  Vector first_point1 = *(Vector *) list_get(shape1, 0);
  Vector first_point2 = *(Vector *) list_get(shape2, 0);
  Vector projection1 = {vec_dot(first_point1, projection_line), \
                        vec_dot(first_point1, projection_line)};
  Vector projection2 = {vec_dot(first_point2, projection_line), \
                        vec_dot(first_point2, projection_line)};
  Vector *min_max1 = create_vector_p(projection1);
  Vector *min_max2 = create_vector_p(projection2);

  projection_min_max(shape1, projection_line, min_max1);
  projection_min_max(shape2, projection_line, min_max2);

  double overlaps = 0;

  if (is_between(min_max1->x, min_max2) || \
      is_between(min_max1->y, min_max2) ||
      is_between(min_max2->x, min_max1) ||
      is_between(min_max2->y, min_max1)) {
    overlaps = (min(min_max1->y, min_max2->y) - max(min_max1->x, min_max2->x));
  };

  vector_free(min_max1);
  vector_free(min_max2);

  return overlaps;
}

void projection_min_max(List *shape, Vector projection_line, Vector *min_max) {
  /*
   * projecting a shape onto a line is simply the shape's vertices dotted with
   * the line you wish to project onto
   */
  for (size_t i = 0; i < list_size(shape); i++) {
    double projection_chunk = vec_dot(*(Vector *) list_get(shape, i), \
                                      projection_line);
    if (projection_chunk < min_max->x) {
      min_max->x = projection_chunk;
    }

    else if (projection_chunk > min_max->y) {
      min_max->y = projection_chunk;
    }
  }

}
