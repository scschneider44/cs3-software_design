#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <stdbool.h>
#include "list.h"
#include "vector.h"
#include <math.h>

/**
 * Represents the status of a collision between two shapes.
 * The shapes are either not colliding, or they are colliding along some axis.
 */
typedef struct {
    /** Whether the two shapes are colliding */
    bool collided;
    /**
     * If the shapes are colliding, the axis they are colliding on.
     * This is a unit vector pointing from the first shape towards the second.
     * Normal impulses are applied along this axis.
     * If collided is false, this value is undefined.
     */
    Vector axis;
} CollisionInfo;


/**
 * Determines whether two convex polygons intersect.
 * The polygons are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding
 */
Vector find_collision(List *shape1, List *shape2);

/**
 * Determines whether two convex polygons intersect on one shapes' projection
 * lines. The polygons are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shape1's axes' projection lines all contain overlaps
 */
Vector check_shape_axes(List *shape1, List *shape2, double *min_overlap);

/**
 * Gets the line which is perpendicular to the side of a shape in the form of a
 * vector. The points are 2 points from a shape which make an edge.
 *
 * @param point1 the first point that forms the edge
 * @param point2 the second point that forms the edge
 * @return the line to project on in the form of a Vector
 */
Vector get_projection_line(Vector *point1, Vector *point2);

/**
 * Given an pojection line, determines whether two convex polygons' projections
 * overlap. the polygons are given as lists of vertices in counterclockwise
 * order. There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @param projection_line the line to project the shape onto
 * @return whether the shapes overlap on the given projection
 */
double overlap(List *shape1, List *shape2, Vector projection_line);

/**
 * Changes the given vector to be the min and max of the shape's projection on
 * a given projection line
 * The polygon are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the shape
 * @param projection_line the line to project the shape onto
 * @param min_max the vector that will hold the min and max
 */
void projection_min_max(List *shape, Vector projection_line, Vector *min_max);

/**
 * Determines whether a double is within the x and y of a vector.
 *
 * @param num a number
 * @param vec the vector to check whether a number is within its bounds
 * @return whether the number is between the x and y of the vec
 */
bool is_between(double num, Vector *vec);


#endif // #ifndef __COLLISION_H__
