/**
 * @file state_syncer.cpp
 * Definitions for a state syncer
 */

#include "state/state_syncer/state_syncer.h"
#include "state/actor/soldier_states/soldier_state.h"
#include <algorithm>
#include <cmath>

namespace state {

StateSyncer::StateSyncer(std::unique_ptr<IState> state, logger::ILogger *logger)
    : state(std::move(state)), logger(logger) {}

void StateSyncer::ExecutePlayerCommands(
    const std::vector<PlayerState *> &player_states,
    const std::vector<bool> &skip_player_commands_flags) {
	auto state_soldiers = state->GetAllSoldiers();
	auto state_towers = state->GetAllTowers();
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {
		if (skip_player_commands_flags[player_id] == false) {
			int64_t soldier_index = 0;
			int64_t tower_index = 0;
			for (auto const &soldiers :
			     player_states[player_id]->soldiers[player_id]) {
				// A soldier can only either move, attack tower or another
				// soldier
				bool is_attacking_tower = soldiers.tower_target != -1;
				bool is_attacking_soldier = soldiers.soldier_target != -1;
				bool is_moving =
				    soldiers.destination != physics::Vector(-1, -1);

				std::vector<bool> flags{is_attacking_soldier, is_moving,
				                        is_attacking_tower};

				if (std::count(flags.begin(), flags.end(), true) > 1) {
					LogErrors(static_cast<PlayerId>(player_id), 1,
					          "Soldier can perform only one task each turn");
				} else {
					if (is_attacking_tower) {
						AttackTower(static_cast<PlayerId>(player_id),
						            soldiers.id, soldiers.tower_target,
						            soldier_index);
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
				soldier_index++;
			}

			for (auto const &tower :
			     player_states[player_id]->towers[player_id]) {
				if (tower.upgrade_tower == true && tower.suicide == true) {
					LogErrors(static_cast<PlayerId>(player_id), 2,
					          "Tower can perform only one task each turn");
				} else if (tower.upgrade_tower == true) {
					UpgradeTower(static_cast<PlayerId>(player_id), tower.id,
					             tower_index);
				} else if (tower.suicide == true) {
					SuicideTower(static_cast<PlayerId>(player_id), tower.id,
					             tower_index);
				}
				tower_index++;
			}

			for (int j = 0; j < player_states[player_id]->map.size(); ++j) {
				for (int k = 0; k < player_states[player_id]->map[j].size();
				     ++k) {
					if (player_states[player_id]->map[j][k].build_tower ==
					    true) {
						BuildTower(static_cast<PlayerId>(player_id),
						           physics::Vector(j, k));
					}
				}
			}
		}
	}
}

std::vector<int64_t> StateSyncer::UpdateMainState() {
	state->Update();

	return std::vector<int64_t>(1, 1);
}

void StateSyncer::UpdatePlayerStates(
    std::vector<PlayerState *> &player_states) {

	auto state_soldiers = state->GetAllSoldiers();
	auto state_towers = state->GetAllTowers();
	auto *map = state->GetMap();
	auto state_money = state->GetMoney();

	// Assigning values from map of state to map of player_state
	std::vector<std::vector<PlayerMapElement>> player_map;

	for (int i = 0; i < map->GetSize(); ++i) {
		std::vector<PlayerMapElement> map_element_vector;
		for (int j = 0; j < map->GetSize(); ++j) {
			// Init map properties from state
			PlayerMapElement map_element;
			state::MapElement state_map_element =
			    map->GetElementByOffset(physics::Vector(i, j));

			map_element.terrain = state_map_element.GetTerrainType();

			auto ownerships = state_map_element.GetOwnership();
			std::copy(ownerships.begin(), ownerships.end(),
			          map_element.territory.begin());

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

		for (int i = 0; i < state_soldiers.size(); ++i) {
			for (int j = 0; j < state_soldiers[i].size(); ++j) {
				// Reassigns id to all the soldiers
				player_states[player_id]->soldiers[i][j].id =
				    state_soldiers[i][j]->GetActorId();

				// Init targets to default value
				player_states[player_id]->soldiers[i][j].tower_target = -1;
				player_states[player_id]->soldiers[i][j].soldier_target = -1;
				player_states[player_id]->soldiers[i][j].destination =
				    physics::Vector(-1, -1);

				// Init soldier properties from state
				player_states[player_id]->soldiers[i][j].hp =
				    state_soldiers[i][j]->GetHp();
				player_states[player_id]->soldiers[i][j].state =
				    state_soldiers[i][j]->GetState();
				if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
					player_states[player_id]->soldiers[i][j].position =
					    state_soldiers[i][j]->GetPosition();
				else {
					player_states[player_id]->soldiers[i][j].position =
					    FlipPosition(map, state_soldiers[i][j]->GetPosition());
				}
			}
		}

		for (int i = 0; i < state_towers.size(); ++i) {
			for (int j = 0; j < state_towers[i].size(); ++j) {
				// Reassigns id to all the towers
				player_states[player_id]->towers[i][j].id =
				    state_towers[i][j]->GetActorId();

				// Init tower writables to default value
				player_states[player_id]->towers[i][j].suicide = false;
				player_states[player_id]->towers[i][j].upgrade_tower = false;

				// Init tower properties from state
				player_states[player_id]->towers[i][j].hp =
				    state_towers[i][j]->GetHp();
				if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
					player_states[player_id]->towers[i][j].position =
					    state_towers[i][j]->GetPosition();
				else {
					player_states[player_id]->towers[i][j].position =
					    FlipPosition(map, state_towers[i][j]->GetPosition());
				}
				player_states[player_id]->towers[i][j].level =
				    state_towers[i][j]->GetTowerLevel();
			}
			player_states[player_id]->num_towers[i] = state_towers[i].size();
		}

		// Checks if map element is valid for building tower and assigns bool
		// value
		for (auto &player_map_row : player_map) {
			for (auto &player_map_element : player_map_row) {
				player_map_element.valid_territory = true;
				for (int i = 0; i < player_map_element.territory.size(); ++i) {
					if (player_map_element.territory[i] == true &&
					    i != player_id) {
						player_map_element.valid_territory = false;
					}
					if (i == player_id &&
					    player_map_element.territory[player_id] == false)
						player_map_element.valid_territory = false;
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

void StateSyncer::FlipMap(
    std::vector<std::vector<PlayerMapElement>> &player_map) {
	int64_t map_size = player_map.size();
	for (int i = 0; i < map_size / 2; ++i) {
		for (int j = 0; j < player_map[i].size(); ++j) {
			std::swap(player_map[i][j],
			          player_map[map_size - 1 - i][map_size - 1 - j]);
		}
	}
	if (map_size % 2) {
		int64_t i = map_size / 2;
		for (int j = 0; j < map_size / 2; ++j) {
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

void StateSyncer::LogErrors(PlayerId player_id, int64_t error_code,
                            std::string message) {
	logger->LogError(player_id, error_code, message);
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
		LogErrors(player_id, 3, "Do not alter id of actors");
		return;
	}

	// Check is soldier is alive to act
	if (state_soldiers[static_cast<int>(player_id)][soldier_index]->GetHp() ==
	    0) {
		LogErrors(player_id, 4, "Soldier must be alive in order to act");
		return;
	}

	// Check if position is valid
	if (position.x < 0 ||
	    position.x >= map->GetSize() * map->GetElementSize() ||
	    position.y < 0 ||
	    position.y >= map->GetSize() * map->GetElementSize()) {
		LogErrors(player_id, 5, "Position not in map");
		return;
	}

	state->MoveSoldier(player_id, soldier_id, position);
}

void StateSyncer::AttackTower(PlayerId player_id, int64_t soldier_id,
                              int64_t tower_id, int64_t soldier_index) {
	bool valid_target = false;
	int64_t opponent_id = (static_cast<int>(player_id) + 1) %
	                      static_cast<int>(PlayerId::PLAYER_COUNT);
	auto state_soldiers = state->GetAllSoldiers();

	// Check if id has been altered
	if (soldier_id !=
	    state_soldiers[static_cast<int>(player_id)][soldier_index]
	        ->GetActorId()) {
		LogErrors(player_id, 3, "Do not alter id of actors");
		return;
	}

	// Check is soldier is alive to act
	if (state_soldiers[static_cast<int>(player_id)][soldier_index]->GetHp() ==
	    0) {
		LogErrors(player_id, 4, "Soldier must be alive in order to act");
		return;
	}

	std::vector<Tower *> opponent_towers = state->GetAllTowers()[opponent_id];

	// Check if target  is valid
	for (int i = 0; i < opponent_towers.size(); ++i) {
		// Check if opponent actor id is correct id
		if (tower_id == opponent_towers[i]->GetActorId())
			valid_target = true;
	}
	if (!valid_target) {
		LogErrors(player_id, 6, "Attack Opponent's tower only");
		return;
	}

	state->AttackActor(player_id, soldier_id, tower_id);
}

void StateSyncer::AttackSoldier(PlayerId player_id, int64_t soldier_id,
                                int64_t enemy_soldier_id,
                                int64_t soldier_index) {
	bool valid_target = false;
	bool opponent_alive = false;
	int64_t opponent_id = (static_cast<int>(player_id) + 1) %
	                      static_cast<int>(PlayerId::PLAYER_COUNT);
	auto state_soldiers = state->GetAllSoldiers();
	std::vector<Soldier *> opponent_soldiers =
	    state->GetAllSoldiers()[opponent_id];

	// Check if id has been altered
	if (soldier_id !=
	    state_soldiers[static_cast<int>(player_id)][soldier_index]
	        ->GetActorId()) {
		LogErrors(player_id, 3, "Do not alter id of actors");
		return;
	}

	// Check if soldier is alive to act
	if (state_soldiers[static_cast<int>(player_id)][soldier_index]->GetHp() ==
	    0) {
		LogErrors(player_id, 4, "Soldier must be alive in order to act");
		return;
	}

	// Check if opponent actor id is correct id
	for (int i = 0; i < opponent_soldiers.size(); ++i) {
		if (enemy_soldier_id == opponent_soldiers[i]->GetActorId()) {
			valid_target = true;
			// Check if opponent soldier is alive.
			if (opponent_soldiers[i]->GetHp() != 0)
				opponent_alive = true;
		}
	}

	if (!valid_target) {
		LogErrors(player_id, 7, "Attack Opponent's soldier only");
		return;
	}
	if (!opponent_alive) {
		LogErrors(player_id, 8, "Opponent soldier must be alive to attack it");
		return;
	}

	state->AttackActor(player_id, soldier_id, enemy_soldier_id);
}

void StateSyncer::BuildTower(PlayerId player_id, physics::Vector offset) {
	bool valid_territory = true;
	auto *map = state->GetMap();
	auto state_money = state->GetMoney();
	auto state_towers = state->GetAllTowers();
	MapElement tower_element = map->GetElementByOffset(offset);
	// Flip position for Player2
	if (player_id == PlayerId::PLAYER2) {
		offset.x = map->GetSize() - 1 - offset.x;
		offset.y = map->GetSize() - 1 - offset.y;
	}

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
		LogErrors(player_id, 9, "Tower can be built only on valid territory.");
		return;
	}

	state->BuildTower(player_id, offset);
}

void StateSyncer::UpgradeTower(PlayerId player_id, int64_t tower_id,
                               int64_t tower_index) {
	auto state_towers = state->GetAllTowers();
	// Check if id has been altered.
	if (tower_id !=
	    state_towers[static_cast<int>(player_id)][tower_index]->GetActorId()) {
		LogErrors(player_id, 3, "Do not alter id of actors");
		return;
	}
	state->UpgradeTower(player_id, tower_id);
}

void StateSyncer::SuicideTower(PlayerId player_id, int64_t tower_id,
                               int64_t tower_index) {
	auto state_towers = state->GetAllTowers();
	// Check if id has been altered.
	if (tower_id !=
	    state_towers[static_cast<int>(player_id)][tower_index]->GetActorId()) {
		LogErrors(player_id, 3, "Do not alter id of actors");
		return;
	}
	state->SuicideTower(player_id, tower_id);
}
}
