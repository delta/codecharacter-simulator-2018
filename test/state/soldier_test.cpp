#include <cstdint>
#include "state/actor/respawn_system/respawn_system.h"
#include "state/actor/soldier.h"
#include "state/actor/actor.h"
#include "state/map/map.h"
#include "state/utilities.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class SoldierTest : public Test {
protected:

	unique_ptr<RespawnSystem> respawn_system;
	unique_ptr<Soldier> soldier;

	SoldierTest()
	: soldier(make_unique<Soldier>(
		1,
		state::PlayerId::PLAYER1,
		state::ActorType::SOLDIER,
		100,
		100,
		physics::Vector(10,10),
		50,
		physics::Vector(15,15)
	) // make_unique
	) {
		this->respawn_system = make_unique<RespawnSystem>(
			soldier.get()
		);
	}
};

TEST_F(SoldierTest, RespawnTest) {

	// Kill the soldier
	soldier->SetHp(0);

	// Run some updates of the soldier
	// until the turn before it respawns
	for(int i = 0; i < RespawnSystem::total_turns_to_respawn; ++i) {
		soldier->Update();
		ASSERT_EQ(soldier->GetHp(), 0);
	}

	// Soldier should respawn
	soldier->Update();
	ASSERT_EQ(soldier->GetHp(), 100);
	ASSERT_EQ(soldier->GetPosition(), Vector(40,40));
}