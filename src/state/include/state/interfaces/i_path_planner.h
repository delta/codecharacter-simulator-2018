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
	 * Gets the subsequent node in a path, given a source and a destination
	 *
	 * @param[in]   source      A Vector node, with the current position
	 * @param[in]   destination A Vector node, with the destination position
	 *
	 * @return      A Vector element, with the next node to traverse to
	 */
	virtual physics::Vector GetNextNode(const physics::Vector &source,
	                                    const physics::Vector &destination) = 0;
};

#endif
