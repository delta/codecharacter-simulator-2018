/**
 * @file path_planner.cpp
 * Defines the PathPlanner data structure for path planning
 */

#include "state/path_planner/path_planner.h"
#include <exception>
#include <memory>

namespace state {

PathPlanner::PathPlanner(Map *map) {
	// Initialise Members
	this->map_size = map->GetSize();
	this->map = map;
	this->adjacency_list = init_matrix(std::list<physics::Vector>(), map_size);
	this->paths =
	    init_matrix(init_matrix(physics::Vector(), map_size), map_size);

	// Set Edges into the Adjacency List
	for (int i = 0; i < map->GetSize(); ++i) {
		for (int j = 0; j < map->GetSize(); ++j) {
			MapElement element = map->GetElementByOffset(physics::Vector(i, j));
			if (element.GetTerrainType() == TerrainType::LAND) {
				this->adjacency_list[i][j] =
				    FindNeighbors(physics::Vector(i, j), map);
			} else if (element.GetTerrainType() == TerrainType::WATER) {
				continue;
			}
		}
	}

	// Populate the paths array with next node data
	for (int i = 0; i < map->GetSize(); ++i) {
		for (int j = 0; j < map->GetSize(); ++j) {
			paths[i][j] = ComputeAllPathsFromNode(physics::Vector(i, j));
		}
	}
}

std::list<physics::Vector> PathPlanner::FindNeighbors(physics::Vector elem,
                                                      Map *map) {
	int x, y;

	std::list<physics::Vector> neighbors;
	std::list<physics::Vector> rel_neighbors;

	// Choose the 4 Adjacent Nodes
	rel_neighbors.emplace_back(0, 1);
	rel_neighbors.emplace_back(0, -1);
	rel_neighbors.emplace_back(1, 0);
	rel_neighbors.emplace_back(-1, 0);

	for (auto neighbor_pos : rel_neighbors) {
		if (neighbor_pos.x == 0 && neighbor_pos.y == 0)
			continue;
		x = elem.x + neighbor_pos.x;
		y = elem.y + neighbor_pos.y;
		if (x >= 0 && x < map_size && y >= 0 && y < map_size &&
		    map->GetElementByOffset(elem).GetTerrainType() ==
		        TerrainType::LAND) {
			neighbors.emplace_back(x, y);
		}
	}
	return neighbors;
}

matrix<physics::Vector>
PathPlanner::ComputeAllPathsFromNode(physics::Vector node) {
	std::queue<physics::Vector> queue;
	auto visited = init_matrix(false, map_size);

	// A matrix which for each element, contains the node that comes
	// next in the path. By following the nodeset for a particular
	// destination from a source, we can obtain the path.
	//
	// Each element of this->paths will be the next_node_matrix belonging
	// to that particular element. This function will generate the
	// next_node_matrix corresponding to the argument passed
	auto next_node_matrix = init_matrix(physics::Vector(), map_size);
	next_node_matrix[node.x][node.y] = node;

	// BFS All Nodes
	// Add the first node
	queue.push(node);
	visited[node.x][node.y] = true;

	// Variable to store the node currently being searched on
	physics::Vector current;

	// While there are no new nodes to visit
	while (!queue.empty()) {
		// Get the next node from the queue
		current = queue.front();
		queue.pop();
		// Iterate through all the neighbors of the current node
		for (const auto &neighbor : adjacency_list[current.x][current.y]) {
			if (visited[neighbor.x][neighbor.y])
				continue;
			// Visit the neighbor
			visited[neighbor.x][neighbor.y] = true;
			next_node_matrix[neighbor.x][neighbor.y] = current;
			queue.push(neighbor);
		}
	}
	return next_node_matrix;
}

physics::Vector PathPlanner::GetNextNode(const physics::Vector &source,
                                         const physics::Vector &destination) {
	// Bounds Checks
	if (source.x < 0 || source.y < 0 || source.x >= map_size ||
	    source.y >= map_size) {
		throw std::out_of_range("Source node out of range");
	}

	if (destination.x < 0 || destination.y < 0 || destination.x >= map_size ||
	    destination.y >= map_size) {
		throw std::out_of_range("Destination node out of range");
	}

	// Terrain Checks
	if (this->map->GetElementByOffset(source).GetTerrainType() !=
	    TerrainType::LAND) {
		throw std::out_of_range("Source node is of invalid terrain type");
	}

	if (this->map->GetElementByOffset(destination).GetTerrainType() !=
	    TerrainType::LAND) {
		throw std::out_of_range("Destination node is of invalid terrain type");
	}

	// Return the next node in the list
	return paths[destination.x][destination.y][source.x][source.y];
}
}
