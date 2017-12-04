#include "gmock/gmock.h"
#include "physics/vector.h"
#include "state/interfaces/i_path_planner.h"
#include <cstdint>

using namespace state;
using namespace physics;

class PathPlannerMock : public IPathPlanner {
  public:
	MOCK_METHOD2(GetNextNode, Vector(const Vector &, const Vector &));
};
