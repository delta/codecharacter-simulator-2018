#include "physics/vector.h"
#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;

/**
 * Tests for Soldier Constructor
 */
TEST(Soldiers, Constructor) {
	unique_ptr<state::Soldier> test_soldier(new state::Soldier(
		1,
		state::PlayerId::PLAYER1,
		state::ActorType::SOLDIER,
		100,
		100,
		physics::Vector(10,10),
		50,
		physics::Vector(15,15)
	));
	EXPECT_EQ(1, test_soldier->GetActorId());
}
