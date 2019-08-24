#ifndef __VECTOR_H__
#define __VECTOR_H__

/**
 * A real-valued 2-dimensional vector.
 * Positive x is towards the right; positive y is towards the top.
 * Vector is defined here instead of vector.c because it is passed *by value*.
 */
typedef struct vector{
    double x;
    double y;
} Vector;

/**
 * The zero vector, i.e. (0, 0).
 * "extern" declares this global variable without allocating memory for it.
 * You will need to define "const Vector VEC_ZERO = ..." in vector.c.
 */
extern const Vector VEC_ZERO;

/**
 * Initializes a 2D vector with an x component x and a y component y
 *
 * @param x the x-component of the new vector
 * @param y the y-component of the new vector
 * @return a new vector that represents <x, y>
 */
Vector vec_init(double x, double y);

/**
 * Initializes a pointer to a vector object
 *
 * @param v the vector object to create a pointer to
 * @return pointer to a vector with the attributes of v
 */
Vector *create_vector_p(Vector v);

/**
 * Releases the memory allocated for a vector.
 *
 * @param list a pointer to a vector returned from create_vector_p()
 */
void vector_free(void *v);

/**
 * Adds two vectors.
 * Performs the usual componentwise vector sum.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 + v2
 */
Vector vec_add(Vector v1, Vector v2);

/**
 * Subtracts two vectors.
 * Performs the usual componentwise vector difference.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 - v2
 */
Vector vec_subtract(Vector v1, Vector v2);

/**
 * Computes the additive inverse a vector.
 * This is equivalent to multiplying by -1.
 *
 * @param v the vector whose inverse to compute
 * @return -v
 */
Vector vec_negate(Vector v);

/**
 * Multiplies a vector by a scalar.
 * Performs the usual componentwise product.
 *
 * @param scalar the number to multiply the vector by
 * @param v the vector to scale
 * @return scalar * v
 */
Vector vec_multiply(double scalar, Vector v);

/**
 * Computes the dot product of two vectors.
 * See https://en.wikipedia.org/wiki/Dot_product#Algebraic_definition.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 . v2
 */
double vec_dot(Vector v1, Vector v2);

/**
 * Computes the cross product of two vectors,
 * which lies along the z-axis.
 * See https://en.wikipedia.org/wiki/Cross_product#Computing_the_cross_product.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the z-component of v1 x v2
 */
double vec_cross(Vector v1, Vector v2);

/**
 * Rotates a vector by an angle around (0, 0).
 * The angle is given in radians.
 * Positive angles are counterclockwise, according to the right hand rule.
 * See https://en.wikipedia.org/wiki/Rotation_matrix.
 * (You can derive this matrix by noticing that rotation by a fixed angle
 * is linear and then computing what it does to (1, 0) and (0, 1).)
 *
 * @param v the vector to rotate
 * @param angle the angle to rotate the vector
 * @return v rotated by the given angle
 */
Vector vec_rotate(Vector v, double angle);

/**
 * Returns vector projection of v1 onto v2
 * @param  v1 the vector being projected
 * @param  v2 the vector being projected onto
 * @return    the vector projection of v1 onto v2
 */
Vector vec_projection(Vector v1, Vector v2);

/**
 * Returns a unit vector
 * @param  v1 the vector to get the unit vector for
 * @return    the unit vector of v1
 */
Vector vec_unit_vector(Vector v1);

/**
 * Returns the distance between two vectors
 * @param  v1 vector 1
 * @param  v2 vector 2
 * @return    the distance
 */
double vec_distance(Vector v1, Vector v2);

/**
 * Returns magnitude of a vector
 * @param  v the vector
 * @return   the magnitude
 */
double vec_magnitude(Vector v);
double vec_angle(Vector v);
#endif // #ifndef __VECTOR_H__
