#include <cstdint>
#include "gmock/gmock.h"
#include "state/map/interfaces/i_map.h"

class MapMock : public state::IMap {
public:

	MOCK_METHOD1(
		GetElementByXY,
		state::MapElement& (physics::Vector position));

	MOCK_METHOD1(
		GetElementByOffset,
		state::MapElement& (physics::Vector position));

	MOCK_METHOD0(
		GetSize,
		int64_t());

	MOCK_METHOD0(
		GetElementSize,
		int64_t());
};
