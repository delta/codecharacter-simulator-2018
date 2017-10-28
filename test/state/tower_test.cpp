#include "physics/vector.h"
#include "state/actor/tower.h"
#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;

/**
 * Tests for Tower Constructor
 */
TEST(TowerTest, Constructor) {
	unique_ptr<state::Tower> test_tower(
	    new state::Tower(1, state::PlayerId::PLAYER1, state::ActorType::TOWER,
	                     100, 100, physics::Vector(10, 10), false, 1));
	EXPECT_EQ(1, test_tower->GetActorId());
}
