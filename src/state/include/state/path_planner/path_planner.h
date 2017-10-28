/**
 * @file path_planner.h
 * Declares the PathPlanner data structure for path planning
 */

#ifndef STATE_PATH_PLANNER_PATH_GRAPH_H
#define STATE_PATH_PLANNER_PATH_GRAPH_H

#include "physics/vector.h"
#include "state/interfaces/i_path_planner.h"
#include "state/map/map.h"
#include "state/state_export.h"
#include <cstdint>
#include <list>
#include <queue>
#include <vector>

namespace state {

/**
 * 2D matrix
 */
template <typename T> using matrix = std::vector<std::vector<T>>;

/**
 * Initializes matrix with elements of the given value and returns it
 * Takes a single size as the parameter
 *
 * @param[in]  init_value  The initialize value
 * @param[in]  size        The size of the matrix
 *
 * @tparam     T           Data type matrix holds
 *
 * @return     The initialized 2D matrix
 */
template <typename T> matrix<T> init_matrix(T init_value, int64_t size) {
	std::vector<T> row(size, init_value);
	matrix<T> a(size, row);
	return a;
}

/**
 * PathPlanner class
 */
class STATE_EXPORT PathPlanner : public IPathPlanner {
  private:
	/**
	 * Reference to the map object
	 */
	Map *map;

	/**
	 * Size of the map for easy access
	 */
	int64_t map_size;

	/**
	 * Adjacency list with node neighbours, obtained from map
	 */
	matrix<std::list<physics::Vector>> adjacency_list;

	/**
	 * Final computed paths from all nodes to all nodes
	 */
	matrix<matrix<physics::Vector>> paths;

	/**
	 * Add a connection between the two vertices
	 */
	void AddEdge(physics::Vector a, physics::Vector b);

	/**
	 * Returns a list of adjacent elements to an element on the map
	 * Helper function used to build the adjacency list from the map
	 */
	std::list<physics::Vector> FindNeighbors(physics::Vector elem, Map *map);

	/**
	 * Given a node, find the shortest path to it from all other nodes
	 * Implements Breadth First Search
	 */
	matrix<physics::Vector> ComputeAllPathsFromNode(physics::Vector node);

  public:
	/**
	 * Constructor for PathPlanner class
	 */
	PathPlanner(Map *map);

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
	physics::Vector GetNextNode(const physics::Vector &source,
	                            const physics::Vector &dest) override;
};
}

#endif
