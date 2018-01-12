#include "gmock/gmock.h"
#include "state/actor/actor.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/soldier_state.h"
#include "state/actor/tower.h"
#include "state/map/map.h"
#include "state/money_manager/money_manager.h"
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
	unique_ptr<PathPlanner> path_planner;
	unique_ptr<Map> map;
	vector<vector<MapElement>> grid;
	int map_size;
	int elt_size;

	unique_ptr<MoneyManager> money_manager;
	vector<int64_t> player_money;
	int64_t max_money;

	unique_ptr<Soldier> soldier;

	SoldierTest() {
		this->map_size = 10;
		this->elt_size = 5;
		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;
			for (int j = 0; j < map_size; ++j) {
				row.push_back(MapElement(Vector(i * elt_size, j * elt_size),
				                         TerrainType::LAND));
			}
			grid.push_back(row);
		}

		this->map = make_unique<Map>(grid, elt_size);
		this->path_planner = make_unique<PathPlanner>(map.get());

		this->max_money = 10000;
		for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; ++i) {
			player_money.push_back(5000); // Start balance
		}
		this->money_manager =
		    make_unique<MoneyManager>(player_money, max_money);

		this->soldier = make_unique<Soldier>(
		    1, state::PlayerId::PLAYER1, state::ActorType::SOLDIER, 100, 100,
		    physics::Vector(10, 10), 5, 3, 40, path_planner.get(),
		    money_manager.get());
	}
};

TEST_F(SoldierTest, Respawn) {
	// Kill the soldier
	soldier->SetHp(0);

	// Run some updates of the soldier
	// until the turn before it respawns
	for (int i = 0; i < Soldier::total_turns_to_respawn; ++i) {
		soldier->Update();
		soldier->LateUpdate();
		ASSERT_EQ(soldier->GetHp(), 0);
		ASSERT_EQ(soldier->GetState(), SoldierStateName::DEAD);
	}

	// Soldier should respawn
	soldier->Update();
	soldier->LateUpdate();
	ASSERT_EQ(soldier->GetHp(), 100);
	ASSERT_EQ(soldier->GetPosition(), Soldier::respawn_position);
	ASSERT_EQ(soldier->GetState(), SoldierStateName::IDLE);
}

TEST_F(SoldierTest, Movement) {
	int turns = 0;

	// Give the move command
	soldier->Move(Vector(30, 30));
	soldier->Update();
	soldier->LateUpdate();

	// Keep moving till the soldier is idle again
	// The turns check ensures that the test fails in an infinite loop
	while (soldier->GetState() != SoldierStateName::IDLE && turns < 100) {
		ASSERT_EQ(soldier->GetState(), SoldierStateName::MOVE);
		soldier->Update();
		soldier->LateUpdate();
		++turns;
	}

	// Ensure destination is reached
	ASSERT_EQ(soldier->GetState(), SoldierStateName::IDLE);
	ASSERT_EQ(soldier->GetPosition(), Vector(30, 30));
	ASSERT_LT(turns, 10);
}

TEST_F(SoldierTest, Attack) {
	auto *target_tower = new Tower(2, PlayerId::PLAYER2, ActorType::TOWER, 500,
	                               500, physics::Vector(20, 30), false, 0);
	soldier->Attack(target_tower);
	soldier->Update();
	soldier->LateUpdate();
	int turns = 0;
	ASSERT_EQ(soldier->GetState(), SoldierStateName::PURSUIT);
	while (soldier->GetState() == SoldierStateName::PURSUIT && turns < 100) {
		soldier->Update();
		soldier->LateUpdate();
		++turns;
	}
	ASSERT_LT(turns, 8);
	ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);

	// Number of turns it's going to take to destroy the tower
	// Add one turn to account for the state transition after hitting 0
	turns =
	    1 + ceil((double)target_tower->GetHp() / soldier->GetAttackDamage());

	// Let the soldier destroy the tower
	while (turns--) {
		ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);
		soldier->Update();
		soldier->LateUpdate();
	}
	// Check for return to Idle state
	ASSERT_EQ(soldier->GetState(), SoldierStateName::IDLE);
	ASSERT_EQ(money_manager->GetBalance(PlayerId::PLAYER1),
	          player_money[0] + MoneyManager::tower_kill_reward_amount);
	ASSERT_FALSE(soldier->IsAttackTargetSet());
}

TEST_F(SoldierTest, MovingEnemyPursuit) {
	auto *target_soldier =
	    new Soldier(1, state::PlayerId::PLAYER2, state::ActorType::SOLDIER, 100,
	                100, physics::Vector(20, 20), 5, 5, 40, path_planner.get(),
	                money_manager.get());
	target_soldier->Move(Vector(30, 30));
	soldier->Attack(target_soldier);

	soldier->Update();
	target_soldier->Update();
	soldier->LateUpdate();
	target_soldier->LateUpdate();

	int turns = 0;
	ASSERT_EQ(soldier->GetState(), SoldierStateName::PURSUIT);
	while (soldier->GetState() == SoldierStateName::PURSUIT && turns < 100) {
		soldier->Update();
		target_soldier->Update();
		soldier->LateUpdate();
		target_soldier->LateUpdate();
		turns++;
	}
	ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);

	// Number of turns it's going to take to destroy the soldier
	// Add one turn to account for the state transition after hitting 0
	turns =
	    1 + ceil((double)target_soldier->GetHp() / soldier->GetAttackDamage());

	// Let the soldier kill the enemy
	while (turns--) {
		ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);
		soldier->Update();
		target_soldier->Update();
		soldier->LateUpdate();
		target_soldier->LateUpdate();
	}
	ASSERT_EQ(target_soldier->GetState(), SoldierStateName::DEAD);
	// Check for return to Idle state
	ASSERT_EQ(soldier->GetState(), SoldierStateName::IDLE);
	ASSERT_FALSE(soldier->IsAttackTargetSet());
}

TEST_F(SoldierTest, MoveThenAttack) {
	auto *target_tower = new Tower(2, PlayerId::PLAYER2, ActorType::TOWER, 500,
	                               500, physics::Vector(20, 30), false, 0);

	// Let the soldier move for a few turns, and then switch it to attack
	soldier->Move(Vector(0, 40));
	for (int i = 0; i < 5; i++) {
		soldier->Update();
		soldier->LateUpdate();
	}

	soldier->Attack(target_tower);
	soldier->Update();
	soldier->LateUpdate();
	int turns = 0;
	ASSERT_EQ(soldier->GetState(), SoldierStateName::PURSUIT);
	while (soldier->GetState() == SoldierStateName::PURSUIT && turns < 100) {
		soldier->Update();
		soldier->LateUpdate();
		turns++;
	}
	ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);

	// Number of turns it's going to take to destroy the tower
	// Add one turn to account for the state transition after hitting 0
	turns =
	    1 + ceil((double)target_tower->GetHp() / soldier->GetAttackDamage());

	// Let the soldier destroy the tower
	while (turns--) {
		ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);
		soldier->Update();
		soldier->LateUpdate();
	}
	// Check for return to Idle state
	ASSERT_EQ(soldier->GetState(), SoldierStateName::IDLE);
	ASSERT_FALSE(soldier->IsAttackTargetSet());
}

TEST_F(SoldierTest, AttackThenMove) {
	auto *target_tower = new Tower(2, PlayerId::PLAYER2, ActorType::TOWER, 500,
	                               500, physics::Vector(20, 30), false, 0);

	// Let the soldier attack for a few turns, then switch it to move
	soldier->Attack(target_tower);
	soldier->Update();
	soldier->LateUpdate();

	int turns = 0;
	ASSERT_EQ(soldier->GetState(), SoldierStateName::PURSUIT);
	while (soldier->GetState() == SoldierStateName::PURSUIT && turns < 100) {
		soldier->Update();
		soldier->LateUpdate();
		turns++;
	}
	ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);

	for (int i = 0; i < 6; i++) {
		soldier->Update();
		soldier->LateUpdate();
	}
	// It should still be attacking at this point
	ASSERT_EQ(soldier->GetState(), SoldierStateName::ATTACK);

	soldier->Move(Vector(0, 40));
	soldier->Update();
	soldier->LateUpdate();
	ASSERT_EQ(soldier->GetState(), SoldierStateName::MOVE);
	ASSERT_FALSE(soldier->IsAttackTargetSet());

	// Let it move to the destination
	turns = 0;
	while (soldier->GetState() == SoldierStateName::MOVE && turns < 100) {
		soldier->Update();
		soldier->LateUpdate();
	}
	ASSERT_EQ(soldier->GetDestination(), Vector(0, 40));

	// Check for return to Idle state
	ASSERT_EQ(soldier->GetState(), SoldierStateName::IDLE);
}

// This tests the case where a soldier attacks an opponent player and
// immediately goes out of range. The LateUpdate system should make this
// independent of update order
TEST_F(SoldierTest, AttackAndRunFairness1) {
	// Case 1 Soldier 1 runs
	// Set soldiers to same position
	auto *soldier2 =
	    new Soldier(1, state::PlayerId::PLAYER2, state::ActorType::SOLDIER, 100,
	                100, physics::Vector(20, 20), 5, 3, 40, path_planner.get(),
	                money_manager.get());
	soldier->SetPosition(physics::Vector(20, 20));

	// Set both to attack the other
	soldier->Attack(soldier2);
	soldier2->Attack(soldier.get());

	soldier->Update();
	soldier2->Update();
	soldier->LateUpdate();
	soldier2->LateUpdate();

	// Soldier 1 moves away
	soldier->Move(physics::Vector(0, 0));

	soldier->Update();
	soldier2->Update();
	soldier->LateUpdate();
	soldier2->LateUpdate();

	// Soldier 1 should have still suffered the second hit
	ASSERT_EQ(soldier->GetHp(),
	          soldier->GetMaxHp() - (2 * soldier2->GetAttackDamage()));
}

TEST_F(SoldierTest, AttackAndRunFairness2) {
	// Case 2 Soldier 2 runs
	// Set soldiers to same position
	auto *soldier2 =
	    new Soldier(1, state::PlayerId::PLAYER2, state::ActorType::SOLDIER, 100,
	                100, physics::Vector(20, 20), 5, 3, 40, path_planner.get(),
	                money_manager.get());
	soldier->SetPosition(physics::Vector(20, 20));

	// Set both to attack the other
	soldier->Attack(soldier2);
	soldier2->Attack(soldier.get());

	soldier->Update();
	soldier2->Update();
	soldier->LateUpdate();
	soldier2->LateUpdate();

	// Soldier 2 moves away
	soldier2->Move(physics::Vector(0, 0));

	soldier->Update();
	soldier2->Update();
	soldier->LateUpdate();
	soldier2->LateUpdate();

	// Soldier 2 should have still suffered the second hit
	ASSERT_EQ(soldier2->GetHp(),
	          soldier2->GetMaxHp() - (2 * soldier->GetAttackDamage()));
}
