/**
 * @file i_path_planner.h
 * Declares the path planner interface, which provides the next node in
 * a path
 */

#ifndef STATE_INTERFACES_I_PATH_PLANNER
#define STATE_INTERFACES_I_PATH_PLANNER

#include "physics/vector.h"
#include "state/state_export.h"

/**
 * IPathPlanner interface to define the ReturnNextNode method
 */
class STATE_EXPORT IPathPlanner {
  public:
	virtual ~IPathPlanner(){};

	/**
	 * Given the source position, destination position, and movement speed,
	 * return the next position to take. Invokes GetNextNode when necessary
	 *
	 * @param[in]     source       The position where the actor currently is
	 * @param[in]     destination  The position where the actor needs to be
	 * @param[in]     speed        The actor's absolute movement speed in units
	 *
	 * @return        The next position where the actor will be
	 */
	virtual physics::Vector GetNextPosition(const physics::Vector &source,
	                                        const physics::Vector &destination,
	                                        int64_t speed) = 0;
};

#endif
