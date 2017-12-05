#include "gmock/gmock.h"
#include "state/actor/actor.h"
#include "state/actor/respawn_system/respawn_system.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_state.h"
#include "state/actor/tower.h"
#include "state/map/map.h"
#include "state/mocks/map_mock.h"
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
	unique_ptr<PathPlanner> path_planner;

	unique_ptr<Map> map;
	vector<vector<MapElement>> grid;
	int map_size;
	int elt_size;

	unique_ptr<Soldier> soldier;

  public:
	SoldierTest() {
		// Init Map
		this->map_size = 20;
		this->elt_size = 10;
		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;
			for (int j = 0; j < map_size; ++j) {
				row.push_back(MapElement(Vector(i * elt_size, j * elt_size),
				                         TerrainType::LAND));
			}
			grid.push_back(row);
		}

		this->map = make_unique<Map>(grid, elt_size);

		// Init Graph
		this->path_planner = make_unique<PathPlanner>(map.get());
		this->soldier = make_unique<Soldier>(
		    1, state::PlayerId::PLAYER1, state::ActorType::SOLDIER, 100, 100,
		    Vector(0, 0), 5, SoldierState::IDLE, 5, 30,
		    this->path_planner.get(), this->map.get());
	}
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

TEST_F(SoldierTest, MovementTest) {
	soldier->Move(physics::Vector(125, 125));
	int i = 0;
	soldier->Update();
	while (soldier->GetState() != SoldierState::IDLE) {
		cerr << "State : ";
		switch (soldier->GetState()) {
			case SoldierState::IDLE:
				cerr << "IDLE" << endl;
				break;
			case SoldierState::MOVE:
				cerr << "MOVE" << endl;
				break;
			case SoldierState::ATTACK:
				cerr << "ATTACK" << endl;
				break;
			case SoldierState::DEAD:
				cerr << "DEAD" << endl;
				break;
		}
		cerr << "Position : " << soldier->GetPosition() << endl << endl;
		soldier->Update();
	}
}

TEST_F(SoldierTest, MoveWhileAttackTest) {
	auto *target_tower = new Tower(2, PlayerId::PLAYER2, ActorType::TOWER, 500,
	                               500, physics::Vector(20, 30), false, 0);
	soldier->Attack(target_tower);
	int i = 0;
	do {
		if (i==15) {
			soldier->Move(physics::Vector(60,60));
		}
		cerr << "Turn : " << i << endl;
		cerr << "State : ";
		switch (soldier->GetState()) {
		case SoldierState::IDLE:
			cerr << "IDLE" << endl;
			break;
		case SoldierState::MOVE:
			cerr << "MOVE" << endl;
			break;
		case SoldierState::ATTACK:
			cerr << "ATTACK" << endl;
			break;
		case SoldierState::DEAD:
			cerr << "DEAD" << endl;
			break;
		}
		cerr << "Position : " << soldier->GetPosition() << endl << endl;
		soldier->Update();
		++i;
	} while (i < 60);
}

TEST_F(SoldierTest, AttackWhileMoveTest) {
	auto *target_tower = new Tower(2, PlayerId::PLAYER2, ActorType::TOWER, 500,
	                               500, physics::Vector(20, 30), false, 0);
	soldier->Move(physics::Vector(50, 50));
	int i = 0;
	do {
		if (i==15) {
			soldier->Attack(target_tower);
		}
		cerr << "Turn : " << i << endl;
		cerr << "State : ";
		switch (soldier->GetState()) {
		case SoldierState::IDLE:
			cerr << "IDLE" << endl;
			break;
		case SoldierState::MOVE:
			cerr << "MOVE" << endl;
			break;
		case SoldierState::ATTACK:
			cerr << "ATTACK" << endl;
			break;
		case SoldierState::DEAD:
			cerr << "DEAD" << endl;
			break;
		}
		cerr << "Position : " << soldier->GetPosition() << endl << endl;
		soldier->Update();
		++i;
	} while (i < 60);
}
