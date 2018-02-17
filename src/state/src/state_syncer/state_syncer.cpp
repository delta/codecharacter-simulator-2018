/**
 * @file state_syncer.cpp
 * Definitions for a state syncer
 */

#include "state/state_syncer/state_syncer.h"
#include "state/actor/soldier_states/soldier_state.h"
#include <algorithm>
#include <cmath>

namespace state {

StateSyncer::StateSyncer(std::unique_ptr<IState> state, logger::ILogger *logger,
                         std::vector<int64_t> tower_build_costs,
                         int64_t max_num_towers)
    : state(std::move(state)), logger(logger),
      tower_build_costs(tower_build_costs), max_num_towers(max_num_towers) {}

void StateSyncer::ExecutePlayerCommands(
    const std::vector<player_state::State *> &player_states,
    const std::vector<bool> &skip_player_commands_flags) {
	auto state_soldiers = state->GetAllSoldiers();
	auto state_towers = state->GetAllTowers();
	auto state_money = state->GetMoney();
	std::vector<int64_t> razed_towers;

	for (int player_id = 0; player_id < player_states.size(); ++player_id) {
		if (skip_player_commands_flags[player_id] == false) {
			for (int64_t tower_index = 0;
			     tower_index < state_towers[player_id].size(); ++tower_index) {
				auto const &tower =
				    player_states[player_id]->towers[tower_index];
				if (tower.upgrade_tower == true && tower.suicide == true) {
					LogErrors(static_cast<PlayerId>(player_id),
					          logger::ErrorType::NO_MULTIPLE_TOWER_TASKS,
					          "Tower with id " + std::to_string(tower.id) +
					              " can perform only one task each turn");
				} else if (tower.upgrade_tower == true) {
					UpgradeTower(static_cast<PlayerId>(player_id), tower.id,
					             tower_index, state_money[player_id]);
				} else if (tower.suicide == true) {
					SuicideTower(static_cast<PlayerId>(player_id), tower.id,
					             tower_index, razed_towers);
				}
			}
		}
	}

	for (int player_id = 0; player_id < player_states.size(); ++player_id) {
		if (skip_player_commands_flags[player_id] == false) {
			int64_t current_num_towers = state_towers[player_id].size();
			for (int64_t soldier_index = 0;
			     soldier_index < state_soldiers[player_id].size();
			     ++soldier_index) {
				auto const &soldiers =
				    player_states[player_id]->soldiers[soldier_index];
				// A soldier can only either move, attack tower or another
				// soldier
				bool is_attacking_tower = soldiers.tower_target != -1;
				bool is_attacking_soldier = soldiers.soldier_target != -1;
				bool is_moving =
				    soldiers.destination != physics::Vector(-1, -1);

				std::vector<bool> flags{is_attacking_soldier, is_moving,
				                        is_attacking_tower};

				if (std::count(flags.begin(), flags.end(), true) > 1) {
					LogErrors(static_cast<PlayerId>(player_id),
					          logger::ErrorType::NO_MULTIPLE_SOLDIER_TASKS,
					          "Soldier with id " + std::to_string(soldiers.id) +
					              " can perform only one task each turn");
				} else {
					if (is_attacking_tower) {
						AttackTower(static_cast<PlayerId>(player_id),
						            soldiers.id, soldiers.tower_target,
						            soldier_index, razed_towers);
					} else if (is_attacking_soldier) {
						AttackSoldier(static_cast<PlayerId>(player_id),
						              soldiers.id, soldiers.soldier_target,
						              soldier_index);
					} else if (is_moving) {
						MoveSoldier(static_cast<PlayerId>(player_id),
						            soldiers.id, soldiers.destination,
						            soldier_index);
					}
				}
			}

			for (int j = 0; j < player_states[player_id]->map.size(); ++j) {
				for (int k = 0; k < player_states[player_id]->map[j].size();
				     ++k) {
					if (player_states[player_id]->map[j][k].build_tower ==
					    true) {
						BuildTower(static_cast<PlayerId>(player_id),
						           physics::Vector(j, k),
						           state_money[player_id], current_num_towers);
					}
				}
			}
		}
	}
}

void StateSyncer::UpdateMainState() { state->Update(); }

void StateSyncer::UpdatePlayerStates(
    std::vector<player_state::State *> &player_states) {

	auto state_soldiers = state->GetAllSoldiers();
	auto state_towers = state->GetAllTowers();
	auto *map = state->GetMap();
	auto state_money = state->GetMoney();

	// Assigning values from map of state to map of player_state
	std::vector<std::vector<player_state::MapElement>> player_map;

	for (int i = 0; i < map->GetSize(); ++i) {
		std::vector<player_state::MapElement> map_element_vector;
		for (int j = 0; j < map->GetSize(); ++j) {
			// Init map properties from state
			player_state::MapElement map_element;
			state::MapElement state_map_element =
			    map->GetElementByOffset(physics::Vector(i, j));

			auto ownerships = state_map_element.GetOwnership();
			map_element.territory = ownerships[0];
			map_element.enemy_territory = ownerships[1];

			// Init map writables to default value
			map_element.build_tower = false;

			map_element_vector.push_back(map_element);
		}
		player_map.push_back(map_element_vector);
	}

	for (int player_id = 0; player_id < player_states.size(); ++player_id) {

		// Flips map for the second player so they are on left bottom corner
		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER2) {
			FlipMap(player_map);
		}

		int64_t enemy_id =
		    (player_id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT);

		AssignTowerAttributes(player_id, player_states[player_id]->towers,
		                      false);
		AssignTowerAttributes(enemy_id, player_states[player_id]->enemy_towers,
		                      true);

		AssignSoldierAttributes(player_id, player_states[player_id]->soldiers,
		                        false);
		AssignSoldierAttributes(enemy_id,
		                        player_states[player_id]->enemy_soldiers, true);

		player_states[player_id]->num_towers = state_towers[player_id].size();
		player_states[player_id]->num_enemy_towers =
		    state_towers[enemy_id].size();

		// Checks if map element is valid for building tower and assigns
		// bool value
		for (auto &player_map_row : player_map) {
			for (auto &player_map_element : player_map_row) {
				player_map_element.valid_territory = true;
				if (player_map_element.territory == false ||
				    player_map_element.enemy_territory == true) {
					player_map_element.valid_territory = false;
					continue;
				}
			}
		}
		// If tower is already present, it is not valid territory
		for (auto &player_tower : state_towers[player_id]) {
			int64_t x, y;
			physics::Vector position;
			if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
				position = player_tower->GetPosition();
			} else {
				position = FlipPosition(map, player_tower->GetPosition());
			}
			physics::Vector offset_position =
			    (position / map->GetElementSize()).floor();
			player_map[offset_position.x][offset_position.y].valid_territory =
			    false;
		}

		// Assigns map taken from state to player state map
		if (player_id == static_cast<int>(PlayerId::PLAYER_COUNT) - 1) {
			for (int i = 0; i < player_map.size(); ++i) {
				std::move(player_map[i].begin(), player_map[i].end(),
				          player_states[player_id]->map[i].begin());
			}
		} else {
			for (int i = 0; i < player_map.size(); ++i) {
				std::copy(player_map[i].begin(), player_map[i].end(),
				          player_states[player_id]->map[i].begin());
			}
		}
		// Assigns money taken from state to player state's state_money
		player_states[player_id]->money = std::move(state_money[player_id]);
	}

	// This turn is now over, update the logs
	logger->LogState(state.get());
}

void StateSyncer::AssignTowerAttributes(
    int64_t id, std::array<player_state::Tower, MAX_NUM_TOWERS> &towers,
    bool is_enemy) {
	auto state_towers = state->GetAllTowers();
	auto *map = state->GetMap();
	int64_t player_id = id;
	for (int i = 0; i < state_towers[id].size(); ++i) {
		// Reassigns id to all the towers
		towers[i].id = state_towers[id][i]->GetActorId();

		// Init tower writables to default value
		towers[i].suicide = false;
		towers[i].upgrade_tower = false;

		// Init tower properties from state
		towers[i].hp = state_towers[id][i]->GetHp();
		towers[i].level = state_towers[id][i]->GetTowerLevel();
		if (is_enemy) {
			player_id = ((id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT));
		}

		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
			towers[i].position = state_towers[id][i]->GetPosition();
		else {
			towers[i].position =
			    FlipPosition(map, state_towers[id][i]->GetPosition());
		}
	}
}

void StateSyncer::AssignSoldierAttributes(
    int64_t id, std::array<player_state::Soldier, NUM_SOLDIERS> &soldiers,
    bool is_enemy) {
	auto state_soldiers = state->GetAllSoldiers();
	auto *map = state->GetMap();
	int64_t player_id = id;

	for (int i = 0; i < state_soldiers[player_id].size(); ++i) {
		// Reassigns id to all the soldiers
		soldiers[i].id = state_soldiers[id][i]->GetActorId();

		// Init targets to default value
		soldiers[i].tower_target = -1;
		soldiers[i].soldier_target = -1;
		soldiers[i].destination = physics::Vector(-1, -1);

		// Init soldier properties from state
		soldiers[i].hp = state_soldiers[id][i]->GetHp();
		switch (state_soldiers[id][i]->GetState()) {
		case SoldierStateName::ATTACK:
			soldiers[i].state = player_state::SoldierState::ATTACK;
			break;
		case SoldierStateName::DEAD:
			soldiers[i].state = player_state::SoldierState::DEAD;
			break;
		case SoldierStateName::IDLE:
			soldiers[i].state = player_state::SoldierState::IDLE;
			break;
		case SoldierStateName::MOVE:
			soldiers[i].state = player_state::SoldierState::MOVE;
			break;
		case SoldierStateName::PURSUIT:
			soldiers[i].state = player_state::SoldierState::PURSUIT;
			break;
		}

		soldiers[i].is_immune = state_soldiers[id][i]->IsInvulnerable();

		if (is_enemy) {
			player_id = ((id + 1) % static_cast<int>(PlayerId::PLAYER_COUNT));
		}

		if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
			soldiers[i].position = state_soldiers[id][i]->GetPosition();
		else {
			soldiers[i].position =
			    FlipPosition(map, state_soldiers[id][i]->GetPosition());
		}
	}
}

void StateSyncer::FlipMap(
    std::vector<std::vector<player_state::MapElement>> &player_map) {
	int64_t map_size = player_map.size();
	for (int i = 0; i < map_size / 2; ++i) {
		for (int j = 0; j < player_map[i].size(); ++j) {
			// Swap territory for Player2
			std::swap(player_map[i][j].territory,
			          player_map[i][j].enemy_territory);
			std::swap(
			    player_map[map_size - 1 - i][map_size - 1 - j].territory,
			    player_map[map_size - 1 - i][map_size - 1 - j].enemy_territory);

			std::swap(player_map[i][j],
			          player_map[map_size - 1 - i][map_size - 1 - j]);
		}
	}
	if (map_size % 2) {
		int64_t i = map_size / 2;
		for (int j = 0; j < map_size / 2; ++j) {
			// Swap territory for Player2
			std::swap(player_map[i][j].territory,
			          player_map[i][j].enemy_territory);
			std::swap(
			    player_map[map_size - 1 - i][map_size - 1 - j].territory,
			    player_map[map_size - 1 - i][map_size - 1 - j].enemy_territory);

			std::swap(player_map[i][j],
			          player_map[map_size - 1 - i][map_size - 1 - j]);
		}
	}
}

physics::Vector StateSyncer::FlipPosition(state::IMap *map,
                                          physics::Vector position) {
	return physics::Vector(
	    map->GetSize() * map->GetElementSize() - 1 - position.x,
	    map->GetSize() * map->GetElementSize() - 1 - position.y);
}

void StateSyncer::LogErrors(PlayerId player_id, logger::ErrorType error_type,
                            std::string message) {
	logger->LogError(player_id, error_type, message);
}

void StateSyncer::MoveSoldier(PlayerId player_id, int64_t soldier_id,
                              physics::Vector position, int64_t soldier_index) {
	auto *map = state->GetMap();
	auto state_soldiers = state->GetAllSoldiers();

	// Flip position for Player2
	if (player_id == PlayerId::PLAYER2)
		position = FlipPosition(map, position);

	// Check if id has been altered
	if (soldier_id !=
	    state_soldiers[static_cast<int>(player_id)][soldier_index]
	        ->GetActorId()) {
		LogErrors(player_id, logger::ErrorType::NO_ALTER_ACTOR_ID,
		          "Do not alter id of actors");
		return;
	}

	// Check is soldier is alive to act
	if (state_soldiers[static_cast<int>(player_id)][soldier_index]->GetHp() ==
	    0) {
		LogErrors(
		    player_id, logger::ErrorType::NO_ACTION_BY_DEAD_SOLDIER,
		    "Soldier with id " +
		        std::to_string(
		            state_soldiers[static_cast<int>(player_id)][soldier_index]
		                .id) +
		        " must be alive in order to act");
		return;
	}

	// Check if position is valid
	if (position.x < 0 ||
	    position.x >= map->GetSize() * map->GetElementSize() ||
	    position.y < 0 ||
	    position.y >= map->GetSize() * map->GetElementSize()) {
		LogErrors(player_id, logger::ErrorType::INVALID_POSITION,
		          "Position (" + std::to_string(position.x) + "," +
		              std::to_string(position.y) + ") not in map");
		return;
	}

	state->MoveSoldier(player_id, soldier_id, position);
}

void StateSyncer::AttackTower(PlayerId player_id, int64_t soldier_id,
                              int64_t tower_id, int64_t soldier_index,
                              const std::vector<int64_t> &razed_towers) {
	bool valid_target = false;
	int64_t enemy_id = (static_cast<int>(player_id) + 1) %
	                   static_cast<int>(PlayerId::PLAYER_COUNT);
	auto state_soldiers = state->GetAllSoldiers();

	// Check if id has been altered
	if (soldier_id !=
	    state_soldiers[static_cast<int>(player_id)][soldier_index]
	        ->GetActorId()) {
		LogErrors(player_id, logger::ErrorType::NO_ALTER_ACTOR_ID,
		          "Do not alter id of actors");
		return;
	}

	// Check is soldier is alive to act
	if (state_soldiers[static_cast<int>(player_id)][soldier_index]->GetHp() ==
	    0) {
		LogErrors(
		    player_id, logger::ErrorType::NO_ACTION_BY_DEAD_SOLDIER,
		    "Soldier with id " +
		        std::to_string(
		            state_soldiers[static_cast<int>(player_id)][soldier_index]
		                .id) +
		        " must be alive in order to act");
		return;
	}

	std::vector<Tower *> enemy_towers = state->GetAllTowers()[enemy_id];

	// Check if target  is valid
	for (int i = 0; i < enemy_towers.size(); ++i) {
		// Check if enemy actor id is correct id
		if (tower_id == enemy_towers[i]->GetActorId()) {
			if (enemy_towers[i]->GetIsBase()) {
				LogErrors(player_id, logger::ErrorType::NO_ATTACK_BASE_TOWER,
				          "Cannot attack Base Tower of enemy");
				return;
			}

			valid_target = true;
		}
	}
	if (!valid_target) {
		LogErrors(player_id, logger::ErrorType::NO_ATTACK_SELF_TOWER,
		          "Attack enemy's tower only");
		return;
	}

	if (find(razed_towers.begin(), razed_towers.end(), tower_id) !=
	    razed_towers.end()) {
		LogErrors(player_id, logger::ErrorType::NO_ATTACK_RAZED_TOWER,
		          "Can't attack tower with id " + std::to_string(tower_id) +
		              " as it has been destroyed by the opponent");
		return;
	}

	state->AttackActor(player_id, soldier_id, tower_id);
}

void StateSyncer::AttackSoldier(PlayerId player_id, int64_t soldier_id,
                                int64_t enemy_soldier_id,
                                int64_t soldier_index) {
	bool valid_target = false;
	bool enemy_alive = false;
	bool enemy_immune = false;
	int64_t enemy_id = (static_cast<int>(player_id) + 1) %
	                   static_cast<int>(PlayerId::PLAYER_COUNT);
	auto state_soldiers = state->GetAllSoldiers();
	std::vector<Soldier *> enemy_soldiers = state->GetAllSoldiers()[enemy_id];

	// Check if id has been altered
	if (soldier_id !=
	    state_soldiers[static_cast<int>(player_id)][soldier_index]
	        ->GetActorId()) {
		LogErrors(player_id, logger::ErrorType::NO_ALTER_ACTOR_ID,
		          "Do not alter id of actors");
		return;
	}

	// Check if soldier is alive to act
	if (state_soldiers[static_cast<int>(player_id)][soldier_index]->GetHp() ==
	    0) {
		LogErrors(
		    player_id, logger::ErrorType::NO_ACTION_BY_DEAD_SOLDIER,
		    "Soldier with id " +
		        std::to_string(
		            state_soldiers[static_cast<int>(player_id)][soldier_index]
		                .id) +
		        " must be alive in order to act");
		return;
	}

	// Check if enemy actor id is correct id
	for (int i = 0; i < enemy_soldiers.size(); ++i) {
		if (enemy_soldier_id == enemy_soldiers[i]->GetActorId()) {
			valid_target = true;
			// Check if enemy soldier is alive.
			if (enemy_soldiers[i]->GetHp() != 0)
				enemy_alive = true;
			// Check if enemy soldier is invulnerable
			if (enemy_soldiers[i]->IsInvulnerable())
				enemy_immune = true;
		}
	}

	if (!valid_target) {
		LogErrors(player_id, logger::ErrorType::NO_ATTACK_SELF_SOLDIER,
		          "Attack enemy's soldier only");
		return;
	}
	if (!enemy_alive) {
		LogErrors(
		    player_id, logger::ErrorType::NO_ATTACK_DEAD_SOLDIER,
		    "Enemy soldier with id " +
		        std::to_string(
		            state_soldiers[static_cast<int>(player_id)][soldier_index]
		                .id) +
		        " must be alive to attack it");
		return;
	}
	if (enemy_immune) {
		LogErrors(
		    player_id, logger::ErrorType::NO_ATTACK_IMMUNE_SOLDIER,
		    "Cannot damage invulnerable soldier with id " +
		        std::to_string(
		            state_soldiers[static_cast<int>(player_id)][soldier_index]
		                .id));
	}

	state->AttackActor(player_id, soldier_id, enemy_soldier_id);
}

void StateSyncer::BuildTower(PlayerId player_id, physics::Vector offset,
                             int64_t &player_money, int64_t &num_towers) {
	// Check for max limit of towers
	if (num_towers + 1 > max_num_towers) {
		LogErrors(player_id, logger::ErrorType::NO_MORE_TOWERS,
		          "No more towers can be built. Max limit reached.");
		return;
	}

	bool valid_territory = true;
	auto *map = state->GetMap();
	auto state_towers = state->GetAllTowers();

	// Flip position for Player2
	if (player_id == PlayerId::PLAYER2) {
		offset.x = map->GetSize() - 1 - offset.x;
		offset.y = map->GetSize() - 1 - offset.y;
	}

	auto tower_element = map->GetElementByOffset(offset);

	if (tower_element.GetOwnership()[static_cast<int>(player_id)] == false)
		valid_territory = false;
	for (int i = 0; i < tower_element.GetOwnership().size(); ++i) {
		if (tower_element.GetOwnership()[i] == true &&
		    i != static_cast<int>(player_id)) {
			valid_territory = false;
		}
	}
	// If tower is already present, it is not valid territory
	for (auto &player_tower : state_towers[static_cast<int>(player_id)]) {
		if ((player_tower->GetPosition() / map->GetElementSize()).floor() ==
		    offset)
			valid_territory = false;
	}

	if (!valid_territory) {
		LogErrors(player_id, logger::ErrorType::INVALID_TERRITORY,
		          "Tower can be built only on valid territory.");
		return;
	}

	// Check if player has sufficient balance
	int64_t tower_cost = tower_build_costs[0];
	if (player_money < tower_cost) {
		LogErrors(player_id, logger::ErrorType::INSUFFICIENT_FUNDS,
		          "Insufficient funds to build tower");
		return;
	}

	player_money = player_money - tower_cost;
	num_towers = num_towers + 1;
	state->BuildTower(player_id, offset);
}

void StateSyncer::UpgradeTower(PlayerId player_id, int64_t tower_id,
                               int64_t tower_index, int64_t &player_money) {
	auto state_towers = state->GetAllTowers();
	// Check if id has been altered.
	if (tower_id !=
	    state_towers[static_cast<int>(player_id)][tower_index]->GetActorId()) {
		LogErrors(player_id, logger::ErrorType::NO_ALTER_ACTOR_ID,
		          "Do not alter id of actors");
		return;
	}

	// Check if tower is upgradable to another level
	int64_t current_tower_level = static_cast<int64_t>(
	    state_towers[static_cast<int>(player_id)][tower_index]
	        ->GetTowerLevel());
	if (current_tower_level == tower_build_costs.size()) {
		LogErrors(player_id, logger::ErrorType::NO_MORE_UPGRADES,
		          "Max level reached, upgrade not allowed for tower with id " +
		              std::to_string(tower_id));
		return;
	}

	// Check if player has sufficient balance
	int64_t tower_upgrade_cost = tower_build_costs[current_tower_level];
	if (player_money < tower_upgrade_cost) {
		LogErrors(player_id, logger::ErrorType::INSUFFICIENT_FUNDS,
		          "Insufficient funds to upgrade tower with id " +
		              std::to_string(tower_id));
		return;
	}

	player_money = player_money - tower_upgrade_cost;
	state->UpgradeTower(player_id, tower_id);
}

void StateSyncer::SuicideTower(PlayerId player_id, int64_t tower_id,
                               int64_t tower_index,
                               std::vector<int64_t> &razed_towers) {
	auto state_towers = state->GetAllTowers();
	// Check if id has been altered.
	if (tower_id !=
	    state_towers[static_cast<int>(player_id)][tower_index]->GetActorId()) {
		LogErrors(player_id, logger::ErrorType::NO_ALTER_ACTOR_ID,
		          "Do not alter id of actors");
		return;
	}

	// Check if tower is base tower
	if (state_towers[static_cast<int>(player_id)][tower_index]->GetIsBase()) {
		LogErrors(player_id, logger::ErrorType::NO_SUICIDE_BASE_TOWER,
		          "Cannot destroy base tower");
		return;
	}
	razed_towers.push_back(tower_id);
	state->SuicideTower(player_id, tower_id);
}

std::vector<int64_t> StateSyncer::GetScores() {
	return this->state->GetScores();
}
}
