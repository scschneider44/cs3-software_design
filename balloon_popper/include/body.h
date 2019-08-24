#ifndef __BODY_H__
#define __BODY_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "vector.h"

#define DEFAULT_MASS 1.0

/**
 * Defines roles in this game
 */
typedef enum {
    PLAYER,
    ENEMY,
    BULLET,
    REMOVE_ON_COLLISION,
    TURN_WHITE_ON_COLLISION,
    NEVER_REMOVE_ON_COLLISION,
} Role;

/**
 * Defines existence state of bodies.
 */
enum {
    NOT_REMOVED,
    REMOVED
};

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 * Bodies can accumulate forces and impulses during each tick.
 * Angular physics (i.e. torques) are not currently implemented.
 */
typedef struct body Body;

/**
 * Contains additional information for a body. For space invader, we have
 * two ints, one to represent the role (enemy or player), and one to represent
 * its state of removal (not removed, to be removed, or removed)
 */
typedef struct bodyInfo BodyInfo;

/**
 * Initializes a body without any info.
 * Acts like body_init_with_info() where info and info_freer are NULL.
 */
Body *body_init(List *shape, double mass, RGBColor color);

/**
 * Initalizes a body info struct
 * @param  role the role of the body (player, enemy, etc.)
 * @return      pointer to body info
 */
BodyInfo* body_info_init(void *role);
void body_rotate_with_velocity(Body* body);
/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, prevents the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param info_freer if non-NULL, a function call on the info to free it
 * @return a pointer to the newly allocated body
 */
Body *body_init_with_info(
    List *shape, double mass, RGBColor color, void *info, FreeFunc info_freer
);

/**
 * Releases the memory allocated for a body.
 *
 * @param b a pointer to a body returned from body_init()
 */
void body_free(void *b);

/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
List *body_get_shape(Body *body);
Body* body_get_colliding_body(Body *body);
void body_set_colliding_body(Body *body, Body* other);
/**
 * Gets the current center of mass of a body.
 * While this could be calculated with polygon_centroid(), that becomes too slow
 * when this function is called thousands of times every tick.
 * Instead, the body should store its current centroid.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
Vector body_get_centroid(Body *body);

/**
 * Gets the current velocity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's velocity vector
 */
Vector body_get_velocity(Body *body);
/**
 * Gets the current accerleration of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's acceleration vector
 */
Vector body_get_acceleration(Body *body);

/**
 * Gets the current elasticity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's elasticity vector
 */
Vector body_get_elasticity(Body *body);

Role body_get_role(Body *body);
/**
 * Gets the mass of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the mass passed to body_init(), which must be greater than 0
 */
double body_get_mass(Body *body);

/**
 * Gets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the color passed to body_init(), as an (R, G, B) tuple
 */
RGBColor body_get_color(Body *body);

/**
 * Gets the current angle of body.
 * @param  body a pointer to a body returned from body_init()
 * @return      the angle
 */
double body_get_angle(Body *body);

/**
 * Gets the information associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the info passed to body_init()
 */
void *body_get_info(Body *body);

/**
 * Gets the time since the last tick
 *
 * @param body a pointer to a body returned from body_init()
 * @return the time since the last tick
 */
double body_get_time_since_last_collision(Body *body);

/**
 * Translates a body to a new position.
 * The position is specified by the position of the body's center of mass.
 *
 * @param body a pointer to a body returned from body_init()
 * @param x the body's new centroid
 */
void body_set_centroid(Body *body, Vector x);

/**
 * Returns area of body. Used for centroid calculation.
 * @param  body a pointer to a body returned from body_init()
 * @return      the area
 */
double body_area(Body* body);

/**
 * Changes a body's velocity (the time-derivative of its position).
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new velocity
 */
void body_set_velocity(Body *body, Vector v);

/**
 * Changes a body's acceleration.
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new acceleration
 */
void body_set_acceleration(Body *body, Vector v);

/**
 * Changes a body's elasticity.
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new elasticity
 */
void body_set_elasticity(Body *body, Vector v);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 */
void body_set_rotation(Body *body, double angle);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about a given point.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 * @param pivot the pivot to rotate the body around
 */
void body_set_rotation_custom(Body *body, double angle, Vector pivot);

/**
 * sets a body's force vector
 *
 * @param body 		the body to alter
 * @param force 	the force to change body's to
 */
void body_set_force(Body *body, Vector force);

/**
 * sets a body's impulse vector
 *
 * @param body 		the body to alter
 * @param impulse 	the impulse to change body's to
 */
void body_set_impulse(Body *body, Vector impulse);

/**
 * sets a body's role
 *
 * @param body 		the body to alter
 * @param role 	the role to change body's to
 */
void body_set_role(Body *body, Role role);

/**
 * sets a body's color
 *
 * @param body 		the body to alter
 * @param color 	the color to change body's to
 */
void body_set_color(Body *body, RGBColor color);

/**
 * sets a body's angle
 *
 * @param body 		the body to alter
 * @param angle 	the angle to change body's to
 */
void body_set_angle(Body *body, double angle);

/**
 * sets a body's time since the last tick
 *
 * @param body 		the body to alter
 * @param time    time since last tick
 */
void body_set_time_since_last_collision(Body *body, double time);
/**
 * Applies a force to a body over the current tick.
 * If multiple forces are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param force the force vector to apply
 */
void body_add_force(Body *body, Vector force);

/**
 * Applies an impulse to a body.
 * An impulse causes an instantaneous change in velocity,
 * which is useful for modeling collisions.
 * If multiple impulses are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param impulse the impulse vector to apply
 */
void body_add_impulse(Body *body, Vector impulse);

/**
 * Updates the body after a given time interval has elapsed.
 * Sets acceleration and velocity according to the forces and impulses
 * applied to the body during the tick.
 * The body should be translated at the *average* of the velocities before
 * and after the tick.
 * Resets the forces and impulses accumulated on the body.
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick(Body *body, double dt);

/**
 * Updates the body after a given time interval has elapsed
 * without forces. Just uses set acc/vels to update position
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick_no_forces(Body *body, double dt);

/**
 * Marks a body for removal--future calls to body_is_removed() will return true.
 * Does not free the body.
 * If the body is already marked for removal, does nothing.
 *
 * @param body the body to mark for removal
 */
void body_remove(Body *body);

/**
 * Returns whether a body has been marked for removal.
 * This function returns false until body_remove() is called on the body,
 * and returns true afterwards.
 *
 * @param body the body to check
 * @return whether body_remove() has been called on the body
 */
bool body_is_removed(Body *body);

/**
 * Calculates centroid of body
 * @param  body a pointer to a body returned from body_init()
 * @return      the (x,y) centroid of the body
 */
Vector body_calculate_centroid(Body* body);

/**
 * Translates a body
 * @param body        a pointer to a body returned from body_init()
 * @param translation amount of distance (x,y) to translate by
 */
void body_translate(Body *body, Vector translation);

#endif // #ifndef __BODY_H__
