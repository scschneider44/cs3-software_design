#ifndef __COMPARATOR_H
#define __COMPARATOR_H

#include <stddef.h>

/**
 * A comparatortakes in two doubles and determines their relationship based
 * on the type of comparator called.
 */
typedef int (*DoubleComparator)(const double a, const double b);

/**
 * Tests to see if a is less than b
 *
 * @a is a double to be compared to b
 * @b is a double to be compared to a
 * @return an int, 1 if a is less than b, 0 otherwise.
 */
int double_less_then(const double a, const double b);

/**
 * Tests to see if a is greater than b
 *
 * @a is a double to be compared to b
 * @b is a double to be compared to a
 * @return an int, 1 if a is greater than b, 0 otherwise.
 */
int double_great_then(const double a, const double b);

#endif /* __COMPARATOR_H */
