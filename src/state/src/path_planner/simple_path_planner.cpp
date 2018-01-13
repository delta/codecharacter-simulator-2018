/**
 * @file simple_path_planner.cpp
 * Defines the SimplePathPlanner class
 */

#include "state/path_planner/simple_path_planner.h"
#include <algorithm>
#include <cmath>

namespace state {

SimplePathPlanner::SimplePathPlanner(IMap *map) : map(map) {}

physics::Vector
SimplePathPlanner::GetNextPosition(const physics::Vector &source,
                                   const physics::Vector &dest, int64_t speed) {

	// The actual destination to move towards (final result)
	physics::Vector new_position;

	// If the soldier is close enough to the destination simply move it there.
	if (source.distance(dest) <= speed) {
		new_position = dest;

	} else {
		// Find the unit vector along the direction of dest
		physics::Vector direction_vector = dest - source;
		physics::Vector unit_vector =
		    direction_vector / direction_vector.magnitude();

		// Multiply speed with the unit vector to get the displacement vector.
		// Add the displacement with the current position to get position
		new_position = source + (unit_vector * speed);
	}

	return new_position;
}
}
