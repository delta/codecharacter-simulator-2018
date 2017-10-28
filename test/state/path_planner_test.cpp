#include "state/map/map.h"
#include "state/path_planner/path_planner.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class PathPlannerTest : public Test {
  protected:
	unique_ptr<PathPlanner> path_planner;

	unique_ptr<Map> map;
	vector<vector<MapElement>> grid;
	int map_size;
	int elt_size;

	PathPlannerTest() {
		// Init Map
		this->map_size = 5;
		this->elt_size = 1;
		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;
			// Map Arrangement :
			// L L L L L
			// L W W W L
			// L L L L L
			// L W W W L
			// L L L L L
			for (int j = 0; j < map_size; ++j) {
				row.push_back(
				    MapElement(Vector(i * elt_size, j * elt_size),
				               (i == 1 || i == 3) && (j >= 1 && j <= 3)
				                   ? TerrainType::WATER
				                   : TerrainType::LAND));
			}
			grid.push_back(row);
		}

		this->map = make_unique<Map>(grid, elt_size);

		// Init Graph
		this->path_planner = make_unique<PathPlanner>(map.get());
	}
};

TEST_F(PathPlannerTest, ValidPathsTests) {
	physics::Vector curr;

	// Standard Path Test
	physics::Vector source(0, 1);
	physics::Vector destination(2, 4);

	// Path :
	// . S>- - |
	// . W W W |
	// . . . . D
	// . W W W .
	// . . . . .
	// Path is 0,1 -> 0,2 -> 0,3 -> 0,4 -> 1,4 -> 2,4

	std::list<physics::Vector> expected_nodes;
	expected_nodes.emplace_back(0, 2);
	expected_nodes.emplace_back(0, 3);
	expected_nodes.emplace_back(0, 4);
	expected_nodes.emplace_back(1, 4);
	expected_nodes.emplace_back(2, 4);

	curr = source;
	for (auto expected_node : expected_nodes) {
		curr = path_planner->GetNextNode(curr, destination);
		ASSERT_EQ(curr, expected_node);
	}

	// Source = Destination test
	source = physics::Vector(2, 2);
	destination = physics::Vector(2, 2);

	// Should return the same postion
	curr = path_planner->GetNextNode(source, destination);
	ASSERT_EQ(curr, source);
}

TEST_F(PathPlannerTest, InvalidPathsTests) {
	// Source or destination is a bad square

	// Invalid Bounds
	EXPECT_THROW(path_planner->GetNextNode(physics::Vector(-1, 0), // Invalid
	                                       physics::Vector(0, 0)   // Valid
	                                       ),
	             std::out_of_range);

	EXPECT_THROW(
	    path_planner->GetNextNode(physics::Vector(0, 0),              // Valid
	                              physics::Vector(map_size, map_size) // Invalid
	                              ),
	    std::out_of_range);

	EXPECT_THROW(path_planner->GetNextNode(physics::Vector(3, -3), // Invalid
	                                       physics::Vector(-1, 1)  // Inavlid
	                                       ),
	             std::out_of_range);

	// Invalid Terrain
	EXPECT_THROW(path_planner->GetNextNode(physics::Vector(3, 3), // Invalid
	                                       physics::Vector(0, 0)  // Valid
	                                       ),
	             std::out_of_range);

	EXPECT_THROW(path_planner->GetNextNode(physics::Vector(0, 0), // Valid
	                                       physics::Vector(1, 1)  // Invalid
	                                       ),
	             std::out_of_range);

	EXPECT_THROW(path_planner->GetNextNode(physics::Vector(3, 3), // Invalid
	                                       physics::Vector(1, 1)  // Inavlid
	                                       ),
	             std::out_of_range);
}
