#include <stdexcept>
#include "physics/vector.h"
#include "state/map/map.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;

class MapTest : public testing::Test {
protected:

	Map map;

	vector<vector<MapElement> > grid;

	int map_size;

	int elt_size;

	MapTest() {
		this->map_size = 60;
		this->elt_size = 30;

		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;

			for (int j = 0; j < map_size; ++j) {
				row.push_back(MapElement(
					Vector(i * elt_size, j * elt_size),
					(j % 2 == 0) ? TerrainType::LAND : TerrainType::WATER));
			}

			grid.push_back(row);
		}

		this->map = Map(grid, elt_size);
	}
};

TEST_F(MapTest, ValidGetElementByXY) {
	Vector position(map_size * elt_size / 2, map_size * elt_size / 2);

	ASSERT_EQ(position, map.GetElementByXY(position).GetPosition());
}

TEST_F(MapTest, InvalidGetElementByXY) {
	Vector excess_pos(map_size * elt_size, map_size * elt_size);
	Vector lack_pos(-1, 0);

	EXPECT_THROW(map.GetElementByXY(excess_pos), std::out_of_range);
	EXPECT_THROW(map.GetElementByXY(lack_pos), std::out_of_range);
}

TEST_F(MapTest, ValidGetElementByOffset) {
	Vector position(map_size / 2, map_size / 2);

	ASSERT_EQ(position, map.GetElementByOffset(position).GetPosition() / elt_size);
}

TEST_F(MapTest, InvalidGetElementByOffset) {
	Vector excess_pos(map_size, map_size);
	Vector lack_pos(-1, 0);

	EXPECT_THROW(map.GetElementByOffset(excess_pos), std::out_of_range);
	EXPECT_THROW(map.GetElementByOffset(lack_pos), std::out_of_range);
}

TEST_F(MapTest, ValidGetSize) {
	ASSERT_EQ(map_size, map.GetSize());
}

TEST_F(MapTest, ValidGetElementSize) {
	ASSERT_EQ(elt_size, map.GetElementSize());
}
