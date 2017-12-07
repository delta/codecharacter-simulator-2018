#include "gmock/gmock.h"
#include "state/actor/actor.h"
#include "state/actor/soldier.h"
#include "state/map/map.h"
#include "state/mocks/path_planner_mock.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class SoldierTest : public Test {
  protected:
	unique_ptr<PathPlannerMock> path_planner;
	unique_ptr<Soldier> soldier;

	SoldierTest()
	    : path_planner(make_unique<PathPlannerMock>()),
	      soldier(make_unique<Soldier>(
	          1, state::PlayerId::PLAYER1, state::ActorType::SOLDIER, 100, 100,
	          physics::Vector(10, 10), 5, 20, 5, path_planner.get())) {}
};
