/**
 * @file state_syncer.cpp
 * Definitions for a state syncer
 */

#include "state/state_syncer/state_syncer.h"
#include <algorithm>

namespace state {

StateSyncer::StateSyncer(std::unique_ptr<IState> state)
    : state(std::move(state)) {}

void StateSyncer::ExecutePlayerCommands(
    const std::vector<PlayerState *> &player_states,
    const std::vector<bool> &skip_player_commands_flags) {
	for (int player_id = 0; player_id < player_states.size(); ++player_id) {
		if (skip_player_commands_flags[player_id] == false) {

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
					LogErrors(1, "Soldier can perform only one task each turn");
				} else {
					if (is_attacking_tower) {
						AttackTower(static_cast<PlayerId>(player_id),
						            soldiers.id, soldiers.tower_target);
					} else if (is_attacking_soldier) {
						AttackSoldier(static_cast<PlayerId>(player_id),
						              soldiers.id, soldiers.soldier_target);
					} else if (is_moving) {
						MoveSoldier(static_cast<PlayerId>(player_id),
						            soldiers.id, soldiers.destination);
					}
				}
			}

			for (auto const &tower :
			     player_states[player_id]->towers[player_id]) {
				if (tower.upgrade_tower == true && tower.suicide == true) {
					LogErrors(1, "Tower can perform only one task each turn.");
				} else if (tower.upgrade_tower == true) {
					UpgradeTower(static_cast<PlayerId>(player_id), tower.id);
				} else if (tower.suicide == true) {
					SuicideTower(static_cast<PlayerId>(player_id), tower.id);
				}
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
				if (player_map_element.terrain == TerrainType::WATER) {
					player_map_element.valid_territory = false;
				} else {
					for (int i = 0; i < player_map_element.territory.size();
					     ++i) {
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
}

void StateSyncer::FlipMap(
    std::vector<std::vector<PlayerMapElement>> &player_map) {
	for (int i = 0; i < (player_map.size() / 2) + 1; ++i) {
		for (int j = 0; j < player_map[i].size(); ++j) {
			PlayerMapElement temp = player_map[i][j];
			player_map[i][j] = player_map[player_map[i].size() - 1 - i]
			                             [player_map[i].size() - 1 - j];
			player_map[player_map[i].size() - 1 - i]
			          [player_map[i].size() - 1 - j] = temp;
		}
	}
}

physics::Vector StateSyncer::FlipPosition(state::IMap *map,
                                          physics::Vector position) {
	physics::Vector truePostion((map->GetSize() - 1) * map->GetElementSize() -
	                                position.x * map->GetElementSize(),
	                            (map->GetSize() - 1) * map->GetElementSize() -
	                                position.y * map->GetElementSize());
	return truePostion;
}

void StateSyncer::LogErrors(int64_t error_code, std::string message) {
	std::cout << error_code << " " << message << "!! \n";
}

void StateSyncer::MoveSoldier(PlayerId player_id, int64_t soldier_id,
                              physics::Vector position) {
	if (player_id == PlayerId::PLAYER2)
		state->MoveSoldier(player_id, soldier_id,
		                   FlipPosition(state->GetMap(), position));
	else
		state->MoveSoldier(player_id, soldier_id, position);
}

void StateSyncer::AttackTower(PlayerId player_id, int64_t soldier_id,
                              int64_t tower_id) {
	bool valid_target = false;
	int64_t opponent_id = (static_cast<int>(player_id) + 1) %
	                      static_cast<int>(PlayerId::PLAYER_COUNT);
	std::vector<Tower *> opponent_towers = state->GetAllTowers()[opponent_id];
	for (int i = 0; i < opponent_towers.size(); ++i) {
		if (tower_id == opponent_towers[i]->GetActorId())
			valid_target = true;
	}
	if (valid_target)
		state->AttackActor(player_id, soldier_id, tower_id);
	else
		LogErrors(2, "Attack Opponent's tower only");
}

void StateSyncer::AttackSoldier(PlayerId player_id, int64_t soldier_id,
                                int64_t enemy_soldier_id) {
	bool valid_target = false;
	int64_t opponent_id = (static_cast<int>(player_id) + 1) %
	                      static_cast<int>(PlayerId::PLAYER_COUNT);
	std::vector<Soldier *> opponent_soldiers =
	    state->GetAllSoldiers()[opponent_id];
	for (int i = 0; i < opponent_soldiers.size(); ++i) {
		if (enemy_soldier_id == opponent_soldiers[i]->GetActorId())
			valid_target = true;
	}
	if (valid_target)
		state->AttackActor(player_id, soldier_id, enemy_soldier_id);
	else
		LogErrors(2, "Attack Opponent's soldier only");
}

void StateSyncer::BuildTower(PlayerId player_id, physics::Vector position) {
	// Goes through all offsets of the map. If player wishes to build a tower,
	// it checks if that is a valid territory for the player
	// and only then builds the tower
	bool valid_territory = true;
	auto *map = state->GetMap();

	if (map->GetElementByOffset(position)
	        .GetOwnership()[static_cast<int>(player_id)] == false)
		valid_territory = false;
	for (int i = 0; i < map->GetElementByOffset(position).GetOwnership().size();
	     ++i) {
		if (map->GetElementByOffset(position).GetOwnership()[i] == true &&
		    i != static_cast<int>(player_id)) {
			valid_territory = false;
		}
	}
	if (valid_territory == false)
		LogErrors(3, "Can be built only on own territory.");
	else {
		if (player_id == PlayerId::PLAYER2) {
			state->BuildTower(player_id,
			                  FlipPosition(state->GetMap(), position));
		} else
			state->BuildTower(player_id, position);
	}
}

void StateSyncer::UpgradeTower(PlayerId player_id, int64_t tower_id) {
	state->UpgradeTower(player_id, tower_id);
}

void StateSyncer::SuicideTower(PlayerId player_id, int64_t tower_id) {
	state->SuicideTower(player_id, tower_id);
}
}
