#include "physics/vector.h"
#include "state/state.h"

#include "gtest/gtest.h"
#include "state/mocks/map_mock.h"

using namespace std;

/**
 * Test for State
 */
TEST(State, SoldierConstructor) {
	auto * test_soldier = new state::Soldier(
		1,
		state::PlayerId::PLAYER1,
		state::ActorType::SOLDIER,
		100,
		100,
		physics::Vector(10,10),
		50,
		physics::Vector(15,15)
	);
	vector<state::Soldier*> test_soldiers;
	test_soldiers.push_back(test_soldier);
	auto * S = new state::State(test_soldiers, new MapMock());
	vector<state::Soldier*> get_soldiers;
	get_soldiers = S->GetAllSoldiers();
	for(int i = 0; i < get_soldiers.size(); i++) {
		EXPECT_EQ(1,   get_soldiers[i]->GetActorId());
		EXPECT_EQ(100, get_soldiers[i]->GetHp());
		EXPECT_EQ(100, get_soldiers[i]->GetMaxHp());
		EXPECT_EQ(50,  get_soldiers[i]->GetMaxSpeed());
	}
}

