#include "gmock/gmock.h"
#include "state/actor/actor.h"
#include "state/actor/soldier.h"
#include "state/map/map.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class SoldierTest : public Test {
  protected:
	unique_ptr<Soldier> soldier;

	SoldierTest()
	    : soldier(make_unique<Soldier>(1, state::PlayerId::PLAYER1,
	                                   state::ActorType::SOLDIER, 100, 100,
	                                   physics::Vector(10, 10), 5, 20, 5)) {}
};
