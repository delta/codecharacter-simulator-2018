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
	 * Given a source node and a destination node, return the next node
	 * that must be taken
	 *
	 * @param[in]     source      The node from which the traversal begins
	 * @param[in]     destination The node to which the traversal is headed
	 *
	 * @return        The next node in the traversal from the source
	 *
	 * @throw         std::out_of_range When source or destination are out of
	 *                                  bounds or have an invalid terrain type
	 */
	virtual physics::Vector GetNextNode(const physics::Vector &source,
	                                    const physics::Vector &destination) = 0;

	/**
	 * Given the source position, destination position, and movement speed,
	 * return the next position to take. Invokes GetNextNode when necessary
	 *
	 * Acts as a convinent wrapper for GetNextNode, performing direct movement
	 * calculations using the speed value
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
