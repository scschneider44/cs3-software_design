#ifndef __UTILS_H__
#define __UTILS_H__

#include "body.h"
#include "vector.h"
#include "scene.h"
#include "comparator.h"
#include "sdl_wrapper.h"
#include <stdbool.h>

extern const double CLOSENESS;

/**
 * Initializes/sets coordinates of the window.
 */
void initialize_window(Vector dimensions);

/**
 * Initializes the scene.
 * @return    the initalized scene
 */
Scene* initialize_scene(void);

/**
* Gets the points of an arrow given a pivot, width, height, and length
*
* @pivot location of the head of the arrow
* @width width of arrowhead
* @height height of arrowhead
* @arrow_length length of arrow
* @return a list of Vectors which are counterclockwise points of an arrow
*/
List* get_arrow_points(Vector pivot, double width, double height, double arrow_length);

/**
* Gets the points of a rectangle given a center, width, and height
*
* @center center of the rectangle
* @width width of rectangle
* @height height of rectangle
* @return a list of Vectors which are counterclockwise points of a rectangle
*/
List* get_rectangle(Vector center, double width, double height);

/**
 * Returns 1 if the two bodies are too close to one another. This function is
 * used when trying to calculate force of gravity because we do not want
 * the force to be so big when r is really small.
 * @param  body1 the first body
 * @param  body2 the second body
 * @return       1 if too close, else 0
 */
int is_too_close(Body* body1, Body* body2);
/**
 * Checks if a polygon is out of bounds. If so, and the velocity of the polygons
 * is toward the bound, the velocity will be negated.
 *
 * @param points a polygon object.
 * @bound coordinates of either the top right or bottom left corner
 * @check_x boolean which determines which bounds to check
 * @comapre function with which to compare the velocity and position of the polygon
 * @elas how much elasticity there is in the "bounce" off the wall
 *
 * @return returns true if a boundary was hit, false otherwise
 */
bool check_out_of_bounds(Body *star, Vector bound, bool check_x, \
    DoubleComparator compare, Vector elas);

/**
* Gets the points of a star given a number of points, a radius, and a posiiton
*
* @num_of_points the number of points the star will have
* @radius the radius of the circle the star will be inscribed on
* @center the coordinates of the center of the star
* @return a list of Vectors which are counterclockwise points of a star
*/
List* get_star_points(size_t num_of_points, double radius, Vector center);

/**
* Gets the points of a star given a number of points, a radius, and a posiiton
*
* @num_of_points the number of points the star will have
* @radius the radius of the circle the star will be inscribed on
* @center the coordinates of the center of the star
* @return a list of Vectors which are counterclockwise points of a star
*/
List* get_bullet_points(Vector center, double height, double width);

/**
* Generates a psedo random decimal.
*
* @min the minimum value the decimal can have
* @max the maximum value the decimal can have
* @return a pseudo random decimal between min and max.
*/
double pseudo_rand_decimal(double min, double max);

/**
* Generates a psedo random integer.
*
* @min the minimum value the integer can have
* @max the maximum value the integer can have
* @return a pseudo random integer between min and max.
*/
int pseudo_rand_int(int min, int max);

/**
 * Returns the points needed to make a circular pellet
 *
 * @param   center_x the x coordinate of the center of the point
 * @param   center_y the y coordinate of the center of the point
 * @param   radius the radius of the circles
 * @param   length_and_height the dimensions of the scene
 * @param   radnom whether or not to randomize the center
 * @return  the points of the circle
 */
List* get_circle_points(Vector center, double radius);

/**
 * returns a random RGBColor
 */
RGBColor rand_color();

/**
 * @param length_and_height		dimentions of the scene
 *
 * returns a random center Vector for a shape
 */
Vector rand_center(Vector length_and_height);

/**
 * Determines whether a double is within the x and y of a vector.
 *
 * @param num a number
 * @param vec the vector to check whether a number is within its bounds
 * @return whether the number is between the x and y of the vec
 */
bool is_between(double num, Vector *vec);

/*
 * returns whether or not two numbers are different signs
 */
bool are_different_signs(double num1, double num2);

/*
 * Given a body, window_dimensions and whether or not all points off, returns 
 * which wall boundary was hit
 */
int which_wall_hit(Body* b, Vector window_dimensions, bool all_points_off);

/**
* Gets the points of an oval given a center, x_span, and y_span
*
* @center the center of the oval
* @x_span length of the x-axis
* @y_span length of the y-axis
* @return a list of Vectors which are counterclockwise points of an oval
*/
List* get_oval_points(Vector center, double x_span, double y_span);

/**
*
* Gets the points of a partial circle (space invader) given a radius, 
* begin, end, and center
*
*/
List* get_partial_circle(double radius, int begin, int end, Vector center);

/*
 * returns the min of two doubles
 */
double min(double a, double b);

/*
 * returns the max of the two doubles
 */
double max(double a, double b);

/**
* Gets the points of a balloon given a center, x_span, and y_span
*
* @center the center of the balloon
* @x_span length of the x-axis
* @y_span length of the y-axis
* @return a list of Vectors which are counterclockwise points of a balloon
*/
List *get_bloon_points(Vector center, double x_span, double y_span);

/**
* Gets the points of a dart given a center, length, and thickness
*
* @center the center of the dart
* @length length of the dart
* @thickness thickness of the dart
* @return a list of Vectors which are counterclockwise points of a dart
*/
List *get_dart_points(Vector center, double length, double thickness);
#endif // ifndef __UTILS_H__
