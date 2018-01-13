#include "state/map/map.h"
#include "state/path_planner/simple_path_planner.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <cmath>
#include <cstdint>

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class SimplePathPlannerTest : public Test {
  protected:
	unique_ptr<SimplePathPlanner> path_planner;

	unique_ptr<Map> map;
	vector<vector<MapElement>> grid;
	int map_size;
	int elt_size;

	SimplePathPlannerTest() {
		// Init Map
		this->map_size = 5;
		this->elt_size = 5;
		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;
			for (int j = 0; j < map_size; ++j) {
				row.push_back(MapElement(Vector(i * elt_size, j * elt_size),
				                         TerrainType::LAND));
			}
			grid.push_back(row);
		}

		this->map = make_unique<Map>(grid, elt_size);

		// Init Graph
		this->path_planner = make_unique<SimplePathPlanner>(map.get());
	}
};

TEST_F(SimplePathPlannerTest, PathTests) {
	Vector source = Vector(2, 2);
	Vector destination = Vector(17, 6);
	int speed = 5;

	auto distance = source.distance(destination);
	int steps = std::ceil(distance / speed);

	Vector temp_source = source;

	for (int i = 0; i < steps; ++i) {
		temp_source =
		    path_planner->GetNextPosition(temp_source, destination, speed);
	}

	EXPECT_EQ(temp_source, destination);
}
