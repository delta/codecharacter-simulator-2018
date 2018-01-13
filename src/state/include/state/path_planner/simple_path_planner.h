/**
 * @file simple_path_planner.h
 * Declares the SimplePathPlanner class for path planning ignoring terrain
 */

#ifndef STATE_PATH_PLANNER_SIMPLE_PATH_PLANNER_H
#define STATE_PATH_PLANNER_SIMPLE_PATH_PLANNER_H

#include "physics/vector.h"
#include "state/interfaces/i_path_planner.h"
#include "state/map/interfaces/i_map.h"
#include "state/state_export.h"
#include <cstdint>
#include <vector>

namespace state {

/**
 * SimplePathPlanner class
 */
class STATE_EXPORT SimplePathPlanner : public IPathPlanner {
  private:
	/**
	 * Reference to the map object
	 */
	IMap *map;

  public:
	/**
	 * Constructor for SimplePathPlanner class
	 */
	SimplePathPlanner(IMap *map);

	/**
	 * @see IPathPlanner#GetNextPosition
	 */
	physics::Vector GetNextPosition(const physics::Vector &source,
	                                const physics::Vector &dest,
	                                int64_t speed) override;
};
}

#endif
