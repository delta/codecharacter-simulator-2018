#include "physics/vector.h"
#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;

/**
 * Tests to make sure tests are testing (testing tests)
 */
TEST(Test, TestingTest) {
	EXPECT_GT(5,2);
}

/**
 * Test for Soldier
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
	auto * S = new state::State(test_soldiers);
	vector<state::Soldier*> get_soldiers;
	get_soldiers = S->GetAllSoldiers();
	for(int i = 0; i < get_soldiers.size(); i++) {
		EXPECT_EQ(1,   get_soldiers[i]->GetActorId());
		EXPECT_EQ(100, get_soldiers[i]->GetHp());
		EXPECT_EQ(100, get_soldiers[i]->GetMaxHp());
		EXPECT_EQ(50,  get_soldiers[i]->GetMaxSpeed());
	}
}



/**
 * Run Tests
 */
int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
