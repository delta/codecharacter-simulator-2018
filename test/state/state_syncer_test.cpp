#include "state/mocks/state_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/state_syncer/state_syncer.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace state;
using namespace physics;
using namespace testing;

class StateSyncerTest : public Test {
  protected:
	unique_ptr<IMap> map;
	vector<vector<MapElement>> grid;
	int map_size;
	int elt_size;

	std::vector<std::vector<Soldier *>> soldiers;

	vector<vector<Tower *>> towers;

	std::vector<int64_t> player_money;

	std::unique_ptr<StateMock> state;

	std::vector<PlayerState *> player_states;

	std::unique_ptr<StateSyncer> state_syncer;

	StateSyncerTest() : state(make_unique<StateMock>()) {

		Actor::SetActorIdIncrement(0);
		// Init PlayerStates
		auto *player_state1 = new PlayerState();
		auto *player_state2 = new PlayerState();

		player_states.push_back(player_state1);
		player_states.push_back(player_state2);

		// Init State Map
		this->map_size = 5;
		this->elt_size = 1;
		for (int i = 0; i < map_size; ++i) {
			vector<MapElement> row;
			// Map Arrangement :
			// L L L L L
			// L L L L L
			// L L L L L
			// L L L L L
			// L L L L L
			for (int j = 0; j < map_size; ++j) {
				row.push_back(MapElement(Vector(i * elt_size, j * elt_size),
				                         TerrainType::LAND));
			}
			this->grid.push_back(row);
		}

		this->map = make_unique<Map>(grid, elt_size);

		// Soldiers for Player1
		std::vector<Soldier *> player1_soldiers;
		for (int i = 0; i < player_state1->soldiers[0].size(); ++i) {
			auto *soldier = new Soldier(
			    Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::SOLDIER,
			    100, 100, Vector(0, 0), 5, 5, 40, nullptr, nullptr);
			player1_soldiers.push_back(soldier);
		}

		// Soldiers for Player 2
		std::vector<Soldier *> player2_soldiers;
		for (int i = 0; i < player_state1->soldiers[0].size(); ++i) {
			auto *soldier = new Soldier(
			    Actor::GetNextActorId(), PlayerId::PLAYER2, ActorType::SOLDIER,
			    100, 100, Vector(this->map_size - 1, this->map_size - 1), 5, 5,
			    40, nullptr, nullptr);
			player2_soldiers.push_back(soldier);
		}

		// Init soldiers for state
		this->soldiers.push_back(player1_soldiers);
		this->soldiers.push_back(player2_soldiers);

		// Init money for state
		for (int i = 0; i < 2; ++i) {
			this->player_money.push_back(5000);
		}

		// Init towers for state
		auto *tower =
		    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER1,
		              ActorType::TOWER, 500, 500, Vector(1, 1), false, 1);
		auto *tower2 = new Tower(
		    Actor::GetNextActorId(), PlayerId::PLAYER2, ActorType::TOWER, 500,
		    500, Vector(this->map_size - 1, this->map_size - 1), false, 1);
		vector<Tower *> player_towers;
		vector<Tower *> player_towers2;
		player_towers.push_back(tower);
		player_towers2.push_back(tower2);
		this->towers.push_back(player_towers);
		this->towers.push_back(player_towers2);

		Vector tower1_position(1, 1);
		this->map->GetElementByOffset(tower1_position)
		    .SetOwnership(PlayerId::PLAYER1, true);
		Vector tower2_position(4, 4);
		this->map->GetElementByOffset(tower2_position)
		    .SetOwnership(PlayerId::PLAYER2, true);

		// towers of playerstates
		//  * * * * 2
		//  * * * * *
		//  * * * * *
		//  * 1 * * *
		//  * * * * *

		// One offset of map owned by both players
		Vector common_position(0, 4);
		this->map->GetElementByOffset(common_position)
		    .SetOwnership(PlayerId::PLAYER1, true);
		this->map->GetElementByOffset(common_position)
		    .SetOwnership(PlayerId::PLAYER2, true);
		this->state_syncer = make_unique<StateSyncer>(state.get());
	}
};

TEST_F(StateSyncerTest, UpdationTest) {
	// Adding another tower to second player to check addition of tower
	auto towers2 = towers;

	auto *tower3 =
	    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER2, ActorType::TOWER,
	              500, 500, Vector(4, 2), false, 1);
	towers2[1].push_back(tower3);

	// towers2
	// + * * * 2
	// * * * * *
	// * * * * 2
	// * 1 * * *
	// * * * * *

	// Removing one tower of second player to check for deletion of towers
	auto towers3 = towers2;
	towers3[1].erase(towers3[1].begin() + 1, towers3[1].end());

	auto player_money2 = player_money;
	player_money2[1] = 4500;

	EXPECT_CALL(*state, GetMap()).WillRepeatedly(Return(map.get()));

	EXPECT_CALL(*state, GetMoney())
	    .WillOnce(Return(player_money))
	    .WillRepeatedly(Return(player_money2));

	EXPECT_CALL(*state, GetAllSoldiers()).WillRepeatedly(Return(soldiers));

	EXPECT_CALL(*state, GetAllTowers())
	    .WillOnce(Return(towers))
	    .WillOnce(Return(towers2))
	    .WillOnce(Return(towers3));

	this->state_syncer->UpdatePlayerStates(player_states);

	// Check for Tower positions for playerstates
	ASSERT_EQ(player_states[1]->towers[1][0].position, Vector(0, 0));
	ASSERT_EQ(player_states[0]->towers[1][0].position,
	          physics::Vector(this->map_size - 1 - 0, this->map_size - 1 - 0));

	// Check for Soldier positions for playerstates
	ASSERT_EQ(player_states[0]->soldiers[0][0].position, Vector(0, 0));
	ASSERT_EQ(player_states[1]->soldiers[0][0].position, Vector(4, 4));

	// Check for valid territory assignment for playerstates map

	// For player1 this is the map (only tower position)
	//  + * * * 2
	//  * * * * *
	//  * * * * *
	//  * 1 * * *
	//  * * * * *
	ASSERT_EQ(player_states[0]->map[1][1].valid_territory, true);
	ASSERT_EQ(player_states[0]->map[4][4].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[4][3].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[4][2].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[0][4].territory[0], true);
	ASSERT_EQ(player_states[0]->map[0][4].territory[1], true);
	ASSERT_EQ(player_states[0]->map[0][4].valid_territory, false);

	// For Player2
	// * * * * *
	// * * * 1 *
	// * * * * *
	// * * * * *
	// 2 * * * +
	ASSERT_EQ(player_states[1]->map[0][0].valid_territory, true);
	ASSERT_EQ(player_states[1]->map[4][4].valid_territory, false);
	ASSERT_EQ(player_states[1]->map[4][3].valid_territory, false);
	ASSERT_EQ(player_states[1]->map[0][2].valid_territory, false);
	ASSERT_EQ(player_states[1]->map[4][0].territory[0], true);
	ASSERT_EQ(player_states[1]->map[4][0].territory[1], true);
	ASSERT_EQ(player_states[1]->map[4][0].valid_territory, false);

	// Check for Money
	ASSERT_EQ(player_states[0]->money, player_money[0]);
	ASSERT_EQ(player_states[1]->money, player_money[1]);

	Vector tower3_position(4, 2);
	this->map->GetElementByOffset(tower3_position)
	    .SetOwnership(PlayerId::PLAYER2, true);

	this->state_syncer->UpdatePlayerStates(player_states);

	// Check for Tower positions for playerstates
	ASSERT_EQ(player_states[1]->towers[1][1].position,
	          physics::Vector(this->map_size - 1 - 4, this->map_size - 1 - 2));
	ASSERT_EQ(player_states[0]->towers[1][1].position, Vector(4, 2));
	ASSERT_EQ(player_states[1]->num_towers[1], 2);
	ASSERT_EQ(player_states[0]->num_towers[0], 1);

	// Check for Soldier positions for playerstates
	ASSERT_EQ(player_states[0]->soldiers[0][0].position, Vector(0, 0));
	ASSERT_EQ(player_states[1]->soldiers[0][0].position, Vector(4, 4));

	// Check for valid territory assignment for playerstates map
	/*
	For player1 this is the map (only tower position)
	 + * * * 2
	 * * * * *
	 * * * * 2
	 * 1 * * *
	 * * * * *
	*/
	ASSERT_EQ(player_states[0]->map[1][1].valid_territory, true);
	ASSERT_EQ(player_states[0]->map[4][2].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[4][3].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[0][0].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[4][4].valid_territory, false);
	ASSERT_EQ(player_states[0]->map[4][4].territory[1], true);
	ASSERT_EQ(player_states[0]->map[4][2].territory[1], true);
	ASSERT_EQ(player_states[0]->map[0][4].territory[0], true);
	ASSERT_EQ(player_states[0]->map[0][4].territory[1], true);
	ASSERT_EQ(player_states[0]->map[0][4].valid_territory, false);

	// For Player2
	/*
	towers2
	 * * * * *
	 * * * 1 *
	 2 * * * *
	 * * * * *
	 2 * * * +
	*/
	ASSERT_EQ(player_states[1]->map[4][2].valid_territory, false);
	ASSERT_EQ(player_states[1]->map[0][2].valid_territory, true);
	ASSERT_EQ(player_states[1]->map[4][3].valid_territory, false);
	ASSERT_EQ(player_states[1]->map[0][0].valid_territory, true);
	ASSERT_EQ(player_states[1]->map[4][4].valid_territory, false);
	ASSERT_EQ(player_states[1]->map[3][3].territory[0], true);
	ASSERT_EQ(player_states[1]->map[4][0].territory[0], true);
	ASSERT_EQ(player_states[1]->map[4][0].territory[1], true);
	ASSERT_EQ(player_states[1]->map[4][0].valid_territory, false);

	// Check for Money
	ASSERT_EQ(player_states[0]->money, player_money2[0]);
	ASSERT_EQ(player_states[1]->money, player_money2[1]);

	this->state_syncer->UpdatePlayerStates(player_states);

	// Player2 has only one tower now
	ASSERT_EQ(player_states[1]->num_towers[1], 1);
	ASSERT_EQ(player_states[0]->num_towers[1], 1);
}

TEST_F(StateSyncerTest, ExecutionTest) {
	player_money[1] = 5000;

	EXPECT_CALL(*state, GetMap()).WillRepeatedly(Return(map.get()));

	EXPECT_CALL(*state, GetMoney()).WillRepeatedly(Return(player_money));

	EXPECT_CALL(*state, GetAllSoldiers()).WillRepeatedly(Return(soldiers));

	auto *tower3 = new Tower(
	    Actor::GetNextActorId(), PlayerId::PLAYER2, ActorType::TOWER, 500, 500,
	    Vector(this->map_size - 1, this->map_size - 1), false, 1);
	towers[1].push_back(tower3);

	EXPECT_CALL(*state, GetAllTowers()).WillRepeatedly(Return(towers));

	Vector tower3_position(4, 4);
	this->map->GetElementByOffset(tower3_position)
	    .SetOwnership(PlayerId::PLAYER2, true);

	this->state_syncer->UpdatePlayerStates(player_states);

	ASSERT_EQ(player_states[1]->num_towers[1], 2);

	vector<bool> skip_player_command_flags;
	skip_player_command_flags.push_back(false);
	skip_player_command_flags.push_back(false);

	// Single soldier targeting soldier and tower
	player_states[0]->soldiers[0][0].tower_target =
	    player_states[0]->towers[1][0].id;
	player_states[0]->soldiers[0][0].soldier_target =
	    player_states[0]->soldiers[1][0].id;

	// Single soldier moving and targeting tower
	player_states[0]->soldiers[0][1].tower_target =
	    player_states[0]->towers[1][0].id;
	player_states[0]->soldiers[0][1].destination = Vector(1, 0);

	// Single soldier moving and targeting soldier
	player_states[1]->soldiers[1][0].soldier_target =
	    player_states[1]->soldiers[0][0].id;
	player_states[1]->soldiers[1][0].destination = Vector(1, 0);

	// Soldier attacking own soldier
	player_states[1]->soldiers[1][1].soldier_target =
	    player_states[1]->soldiers[1][1].id;

	// Make tower suicide and upgrade
	player_states[0]->towers[0][0].upgrade_tower = true;
	player_states[0]->towers[0][0].suicide = true;

	// Trying to build in territory that is not yours
	player_states[0]->map[3][3].build_tower = true;

	// Trying to build in territory that belongs to both
	player_states[0]->map[0][4].build_tower = true;

	// Expect 7 errors to be logged.
	this->state_syncer->ExecutePlayerCommands(player_states,
	                                          skip_player_command_flags);

	// Resetting to default values
	this->state_syncer->UpdatePlayerStates(player_states);

	// Different soldiers given different jobs
	player_states[0]->soldiers[0][0].tower_target =
	    player_states[0]->towers[1][0].id;
	player_states[0]->soldiers[0][1].soldier_target =
	    player_states[0]->soldiers[1][0].id;
	player_states[1]->soldiers[1][1].destination = Vector(4, 3);
	// Different towers given different jobs
	player_states[0]->towers[0][0].upgrade_tower = true;
	player_states[1]->towers[1][1].suicide = true;
	// Building towers
	this->map->GetElementByOffset(Vector(0, 0))
	    .SetOwnership(PlayerId::PLAYER1, true);
	player_states[0]->map[0][0].build_tower = true;
	player_states[0]->map[0][0].territory[0] = true;
	player_states[0]->map[0][0].territory[1] = false;
	player_states[0]->map[0][0].valid_territory = true;

	// Checking if these exact calls are made when executing player commands
	EXPECT_CALL(*state,
	            AttackActor(static_cast<PlayerId>(0),
	                        player_states[0]->soldiers[0][0].id,
	                        player_states[0]->soldiers[0][0].tower_target))
	    .Times(1);
	EXPECT_CALL(*state,
	            AttackActor(static_cast<PlayerId>(0),
	                        player_states[0]->soldiers[0][1].id,
	                        player_states[0]->soldiers[0][1].soldier_target))
	    .Times(1);
	EXPECT_CALL(*state,
	            MoveSoldier(static_cast<PlayerId>(1),
	                        player_states[1]->soldiers[1][1].id, Vector(0, 1)))
	    .Times(1);
	EXPECT_CALL(*state, UpgradeTower(static_cast<PlayerId>(0),
	                                 player_states[0]->towers[0][0].id))
	    .Times(1);
	EXPECT_CALL(*state, SuicideTower(static_cast<PlayerId>(1),
	                                 player_states[1]->towers[1][1].id))
	    .Times(1);
	EXPECT_CALL(*state, BuildTower(static_cast<PlayerId>(0), Vector(0, 0)))
	    .Times(1);

	// Execute the commands
	this->state_syncer->ExecutePlayerCommands(player_states,
	                                          skip_player_command_flags);
}
