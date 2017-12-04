#include "gmock/gmock.h"
#include "state/actor/actor.h"
#include "state/actor/respawn_system/respawn_system.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_state.h"
#include "state/map/map.h"
#include "state/mocks/path_planner_mock.h"
#include "state/path_planner/path_planner.h"
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

  public:
	SoldierTest()
	    : path_planner(make_unique<PathPlannerMock>()),
	      soldier(make_unique<Soldier>(
	          1, state::PlayerId::PLAYER1, state::ActorType::SOLDIER, 100, 100,
	          Vector(10, 10), 5, SoldierState::IDLE, 20, 5,
	          this->path_planner.get())) {}
};

TEST_F(SoldierTest, RespawnTest) {
	// Kill the soldier
	soldier->SetHp(0);

	// Run some updates of the soldier
	// until the turn before it respawns
	for (int i = 0; i < RespawnSystem::total_turns_to_respawn; ++i) {
		soldier->Update();
		ASSERT_EQ(soldier->GetHp(), 0);
		ASSERT_EQ(soldier->GetState(), SoldierState::DEAD);
	}

	// Soldier should respawn
	soldier->Update();
	ASSERT_EQ(soldier->GetHp(), 100);
	ASSERT_EQ(soldier->GetPosition(), Vector(40, 40));
	ASSERT_EQ(soldier->GetState(), SoldierState::IDLE);
}
