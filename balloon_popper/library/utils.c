#include "utils.h"
#include "list.h"
#include <math.h>
#include <stdlib.h>

const double CLOSENESS = 6;

void initialize_window(Vector dimensions) {
    Vector bottom_left = vec_multiply(-0.5, dimensions);
    Vector top_right = vec_multiply(0.5, dimensions);
    sdl_init(bottom_left, top_right);
}

Scene* initialize_scene(void) {
    Scene* scene = scene_init();
    return scene;
}

List* get_arrow_points(Vector pivot, double width, double height, double arrow_length) {
    // This will initialize a horizontal arrow
    size_t number_pts = 7;
    List* points = list_init(number_pts, vector_free);
    Vector top_left = (Vector){pivot.x, pivot.y + height/2};
    Vector bottom_left = (Vector){pivot.x, pivot.y - height/2};
    Vector bottom_right = (Vector){pivot.x + width, pivot.y - height/2};
    Vector bottom_arrow_tip = (Vector){pivot.x + width, pivot.y - height/2 - arrow_length/2};
    Vector end_arrow_tip = (Vector){pivot.x + width + arrow_length, pivot.y};
    Vector top_arrow_tip = (Vector){pivot.x + width, pivot.y + height/2 + arrow_length / 2};
    Vector top_right = (Vector){pivot.x + width, pivot.y + height/2};

    list_add(points, create_vector_p(top_left));
    list_add(points, create_vector_p(bottom_left));
    list_add(points, create_vector_p(bottom_right));
    list_add(points, create_vector_p(bottom_arrow_tip));
    list_add(points, create_vector_p(end_arrow_tip));
    list_add(points, create_vector_p(top_arrow_tip));
    list_add(points, create_vector_p(top_right));
    return points;
}

List* get_rectangle(Vector center, double width, double height) {
    size_t number_pts = 4;
    List* points = list_init(number_pts, vector_free);
    Vector top_left = (Vector){center.x - width/2, center.y + height/2};
    Vector bottom_left = (Vector){center.x - width/2, center.y - height/2};
    Vector top_right = (Vector){center.x + width/2, center.y + height/2};
    Vector bottom_right = (Vector){center.x + width/2, center.y - height/2};
    list_add(points, create_vector_p(top_left));
    list_add(points, create_vector_p(bottom_left));
    list_add(points, create_vector_p(bottom_right));
    list_add(points, create_vector_p(top_right));
    return points;
}

List* get_partial_circle(double radius, int begin, int end, Vector center) {
    size_t number_pts = 50;
    size_t circle_sections = 12;
    List* points = list_init(number_pts, vector_free);
    double angle = 2 * M_PI / number_pts;

    list_add(points, create_vector_p(center));
    for (size_t i = begin * number_pts / circle_sections; \
        i < end * number_pts / circle_sections; i++) {
        Vector vertex = {center.x + radius * cos(i * angle), center.y + \
            radius *sin(i * angle)};
        list_add(points , create_vector_p(vertex));
    }
    return points;
}

List* get_oval_points(Vector center, double x_span, double y_span) {
    size_t number_pts = 52;
    List* points = list_init(number_pts, vector_free);
    double angle = 2 * M_PI / number_pts;
    double height = y_span / 2;
    double width = x_span / 2;
    for (size_t i = 0; i < number_pts; i++) {
          Vector vertex = {center.x + width * cos(i * angle), center.y + \
              height * sin(i * angle)};
          list_add(points , create_vector_p(vertex));
      }
    return points;
}

List* get_bloon_points(Vector center, double x_span, double y_span) {
    size_t number_pts = 104;
    List* points = list_init(number_pts, vector_free);
    double angle = 2 * M_PI / number_pts;
    double height = y_span / 2;
    double width = x_span / 2;
    for (size_t i = 0; i < number_pts; i++) {
      if (i != 78) {
          Vector vertex = {center.x + width * cos(i * angle), center.y + \
              height * sin(i * angle)};
          list_add(points , create_vector_p(vertex));
        }
      else {
        Vector vertex1 = {center.x + width * cos(i * angle) - (x_span / 8), center.y + \
            height * sin(i * angle) - (y_span / 10)};
          list_add(points , create_vector_p(vertex1));
        Vector vertex2 = {center.x + width * cos(i * angle) + (x_span / 8), center.y + \
              height * sin(i * angle) - (y_span / 10)};
            list_add(points , create_vector_p(vertex2));
      }
    }
    return points;
}

List* get_dart_points(Vector tip, double length, double thickness) {
    List* points = list_init(9, vector_free);
    Vector vertex1 = tip;
    list_add(points , create_vector_p(vertex1));
    Vector vertex10 = {tip.x - (length / 6), tip.y + (thickness / 4)};
    list_add(points , create_vector_p(vertex10));
  //  Vector vertex2 = {tip.x - (length / 3), tip.y + (thickness / 2)};
    //list_add(points , create_vector_p(vertex2));
    Vector vertex3 = {tip.x - (length / 3), tip.y + thickness};
    list_add(points , create_vector_p(vertex3));
    Vector vertex4 = {tip.x - ((2 * length) / 3), tip.y + thickness};
    list_add(points , create_vector_p(vertex4));
    Vector vertex12 = {tip.x - ((3 * length) / 4), tip.y + (thickness / 2)};
    list_add(points , create_vector_p(vertex12));
    Vector vertex5 = {tip.x - length, tip.y + thickness * 3};
    list_add(points , create_vector_p(vertex5));
    Vector vertex6 = {tip.x - length, tip.y - thickness * 3};
    list_add(points , create_vector_p(vertex6));
    Vector vertex13 = {tip.x - ((3 * length) / 4), tip.y - (thickness / 2)};
    list_add(points , create_vector_p(vertex13));
    Vector vertex7 = {tip.x - ((2 * length) / 3), tip.y - thickness};
    list_add(points , create_vector_p(vertex7));
    Vector vertex8 = {tip.x - (length / 3), tip.y - thickness};
    list_add(points , create_vector_p(vertex8));
    //Vector vertex9 = {tip.x - (length / 3), tip.y - (thickness / 2)};
    //list_add(points , create_vector_p(vertex9));
    Vector vertex11 = {tip.x - (length / 6), tip.y - (thickness / 4)};
    list_add(points , create_vector_p(vertex11));
    return points;
}

int is_too_close(Body* body1, Body* body2) {
    if (vec_distance(body_get_centroid(body1), \
        body_get_centroid(body2)) < CLOSENESS) {
        return 1;
    }
    return 0;
}


int which_wall_hit(Body* b, Vector window_dimensions, bool all_points_off) {
    List* points = body_get_shape(b);
    int number_walls = 4;
    int wall_counts[] = {0, 0, 0, 0};
    int current_wall;
    for (size_t i = 0; i < list_size(points); i++) {
        current_wall = 0;
        Vector *point = list_get(points, i);
        if (point->x > window_dimensions.x / 2) {
            if (!all_points_off) {
                return RIGHT_WALL;
            }
            current_wall = RIGHT_WALL;
        } else if (point->x < -window_dimensions.x / 2) {
            if (!all_points_off) {
                return LEFT_WALL;
            }
            current_wall = LEFT_WALL;
        } else if (point->y < -window_dimensions.y / 2) {
            if (!all_points_off) {
                return BOTTOM_WALL;
            }
            current_wall = BOTTOM_WALL;
        } else if (point->y > window_dimensions.y / 2) {
            if (!all_points_off) {
                return TOP_WALL;
            }
            current_wall = TOP_WALL;
        } else {
            /* If all_points_off is true, then we require that ALL points are
             * off the screen. Thus if we get any point that is not off the
             * screen, then we know we can return that a shape has not
             * completely left the screen. */
            if (all_points_off)
                return 0;
        }
        if (all_points_off) {
            wall_counts[current_wall-1]++;
        }
    }

    if (all_points_off) {
        // If all points are off the edge
        int max_count = wall_counts[0];
        current_wall = 1;
        for (size_t i = 1; i < number_walls; i++) {
            if (wall_counts[1] > max_count) {
                current_wall = i + 1;
                max_count = wall_counts[1];
            }
        }
        return current_wall;
    }
    return 0;
}

bool are_different_signs(double num1, double num2) {
    return (num1 < 0 && num2 > 0) || (num1 > 0 && num2 < 0);
}

bool check_out_of_bounds(Body *star, Vector bound, bool check_x, \
    DoubleComparator compare, Vector elas) {
  Vector current_velocity = body_get_velocity(star);
  List *star_points = body_get_shape(star);
  for (size_t i = 0; i < list_size(star_points); i++){
    Vector *point = list_get(star_points, i);

    if (check_x) {
      if (compare(point->x, bound.x) && compare(current_velocity.x, 0)) {
        current_velocity.x *= -1 * elas.x;
        body_set_velocity(star, current_velocity);
        return true;
      }
    } else {
      if (compare(point->y, bound.y) && compare(current_velocity.y, 0)) {
        current_velocity.y *= -1 * elas.y;
        body_set_velocity(star, current_velocity);
        return true;
      }
    }
  }
  return false;
}

/* uses trigonometry to get the inner and outer vertices of an n pointed star */
List *get_star_points(size_t num_of_points, double radius, Vector center) {
  List *star = list_init(num_of_points * 2, (FreeFunc) vector_free);
  double angle = M_PI / 2;
  double vertex_shift = M_PI / num_of_points;

  for (size_t i = 0; i < num_of_points; i++) {
    Vector outer_vertex = vec_init(cos(angle), sin(angle));
    Vector update_vec1 = vec_multiply(radius, outer_vertex);
    /* update_vec2 is the vector that adds the inner vertices. This is
     * accomplished by having update_vec1 and update_vec2 being offset by
     * pi / num_of_points
     */
    Vector inner_vertex = vec_init(cos(angle + vertex_shift), \
             sin(angle + vertex_shift));
    Vector update_vec2 = vec_multiply(2 * radius / num_of_points, inner_vertex);
    /* the vertices are 2 * pi / num_of_points rads apart */
    angle += 2 * M_PI / num_of_points;

    Vector outer_point = vec_subtract(center, update_vec1);
    Vector inner_point = vec_subtract(center, update_vec2);
    list_add(star, create_vector_p(outer_point));
    list_add(star, create_vector_p(inner_point));
  }
  return star;
}

List* get_bullet_points(Vector center, double height, double width) {
  size_t number_pts = 4;
  List* points = list_init(number_pts, vector_free);
  Vector v1 = {center.x + (width / 2), center.y + (height / 2)};
  Vector v2 = {center.x - (width / 2), center.y + (height / 2)};
  Vector v3 = {center.x - (width / 2), center.y - (height / 2)};
  Vector v4 = {center.x + (width / 2), center.y - (height / 2)};
  list_add(points, create_vector_p(v1));
  list_add(points, create_vector_p(v2));
  list_add(points, create_vector_p(v3));
  list_add(points, create_vector_p(v4));
  return points;
}

double pseudo_rand_decimal(double min, double max) {
  double range = max - min;
  double rand_decimal = rand() / (double)RAND_MAX;
  rand_decimal = (rand_decimal * range) + min;
  return rand_decimal;
}

int pseudo_rand_int(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}

List* get_circle_points(Vector center, double radius) {
    return get_oval_points(center, radius * 2, radius * 2);
}

RGBColor rand_color() {
    double red_val = pseudo_rand_decimal(0, 1);
    double green_val = pseudo_rand_decimal(0, 1);
    double blue_val = pseudo_rand_decimal(0, 1);
    return (RGBColor){red_val, green_val, blue_val};
}

Vector rand_center(Vector length_and_height) {
    int x_pt = pseudo_rand_int((-1 * length_and_height.x / 2), \
                        (length_and_height.x / 2));
    int y_pt = pseudo_rand_int((-1 * length_and_height.y / 2), \
                        (length_and_height.y / 2));
    return vec_init(x_pt, y_pt);
}

bool is_between(double num, Vector *vec) {
  if (num >= vec->x && num <= vec->y) {
    return true;
  }
  return false;
}

double min(double a, double b) {
  if (a < b) {
    return a;
  }
  return b;
}

double max(double a, double b) {
  if (a > b) {
    return a;
  }
  return b;
}
