#include "polygon.h"
#include "vector.h"
#include "list.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Using the shoestring algorithm to calculate the area of a polygon. */
double polygon_area(List *polygon) {
  double area = 0;
  int length = list_size(polygon);
  /* j starts as the last vector to account for edge case. */
  int j = length - 1;
  for (int i = 0; i < length; i++) {
    area += vec_cross(*((Vector*)list_get(polygon, j)), *((Vector*)list_get(polygon, i)));
    /* j remains one less than i to assure we have adjacent coordinates. */
    j = i;
  }
  return 0.5 * fabs(area);
}

/* Calculating the centroid of a polygon using centroid formula. */
Vector polygon_centroid(List *polygon) {
  double area = (1.0 / (6.0 * polygon_area(polygon)));
  double c_x = 0;
  double c_y = 0;
  int length = list_size(polygon);
  /* j starts as the last vector to account for edge case. */
  int j = length - 1;
  for (int i = 0; i < length; i++) {
    Vector v1 = *((Vector*)list_get(polygon, i));
    Vector v2 = *((Vector*)list_get(polygon, j));
    double cross_prod = vec_cross(v2, v1);
    Vector sum = vec_add(v1, v2);
    c_x += (sum.x * cross_prod);
    c_y += (sum.y * cross_prod);
    /* j remains one less than i to assure we have adjacent coordinates. */
    j = i;
  }
  return (Vector){area * c_x, area * c_y};
}

/* Translates all vertices in the polygon by the input translation vector. */
void polygon_translate(List *polygon, Vector translation) {
  int length = list_size(polygon);
  for (int i = 0; i < length; i++) {
    list_set(polygon, i, create_vector_p(vec_add(*((Vector*)list_get(polygon, i)), translation)));
  }
}

/* Rotates vertices in polygon by input angle about the input point. */
void polygon_rotate(List *polygon, double angle, Vector point) {
  int length = list_size(polygon);
  /* Points to rotate around */
  double x = point.x;
  double y = point.y;
  for (int i = 0; i < length; i++) {
    /* Points in the polygon that we are updating */
    double n_x = ((Vector*)list_get(polygon, i))->x;
    double n_y = ((Vector*)list_get(polygon, i))->y;
    /* Updating the polygon vectors */
    double new_x = cos(angle) * (n_x - x) - sin(angle) * (n_y - y) + x;
    double new_y = cos(angle) * (n_y - y) + sin(angle) * (n_x - x) + y;
    list_set(polygon, i, create_vector_p((Vector){new_x, new_y}));
  }
}
