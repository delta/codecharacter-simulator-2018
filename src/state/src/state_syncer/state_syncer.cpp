/**
 * @file state_syncer.cpp
 * Definitions for a state syncer
 */

#include "state/state_syncer/state_syncer.h"
#include <algorithm>

namespace state {

StateSyncer::StateSyncer(IState *state) : state(state) {}

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
			map_element.territory = state_map_element.GetOwnership();

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

		// Soldiers from state to playerstate
		if (player_states[player_id]->soldiers.size() !=
		    state_soldiers.size()) {
			player_states[player_id]->towers.erase(
			    player_states[player_id]->towers.begin(),
			    player_states[player_id]->towers.end());
			for (int j = 0; j < state_soldiers.size(); ++j) {
				std::vector<PlayerSoldier> new_soldier_vector;
				for (int k = 0; k < state_soldiers[j].size(); ++k) {
					PlayerSoldier new_soldier;
					// Reassigns id to all the soldiers
					new_soldier.id = state_soldiers[j][k]->GetActorId();

					// Init targets to default value
					new_soldier.tower_target = -1;
					new_soldier.soldier_target = -1;
					new_soldier.destination = physics::Vector(-1, -1);

					// Init soldier properties from state
					new_soldier.hp = state_soldiers[j][k]->GetHp();
					new_soldier.state = state_soldiers[j][k]->GetState();
					if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
						new_soldier.position =
						    state_soldiers[j][k]->GetPosition();
					else {
						new_soldier.position = FlipPosition(
						    map, state_soldiers[j][k]->GetPosition());
					}
					new_soldier_vector.push_back(new_soldier);
				}
				player_states[player_id]->soldiers.push_back(
				    new_soldier_vector);
			}
		} else {
			for (int j = 0; j < state_soldiers.size(); ++j) {
				for (int k = 0; k < state_soldiers[j].size(); ++k) {
					// Reassigns id to all the soldiers
					player_states[player_id]->soldiers[j][k].id =
					    state_soldiers[j][k]->GetActorId();

					// Init targets to default value
					player_states[player_id]->soldiers[j][k].tower_target = -1;
					player_states[player_id]->soldiers[j][k].soldier_target =
					    -1;
					player_states[player_id]->soldiers[j][k].destination =
					    physics::Vector(-1, -1);

					// Init soldier properties from state
					player_states[player_id]->soldiers[j][k].hp =
					    state_soldiers[j][k]->GetHp();
					player_states[player_id]->soldiers[j][k].state =
					    state_soldiers[j][k]->GetState();
					if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
						player_states[player_id]->soldiers[j][k].position =
						    state_soldiers[j][k]->GetPosition();
					else {
						player_states[player_id]->soldiers[j][k].position =
						    FlipPosition(map,
						                 state_soldiers[j][k]->GetPosition());
					}
				}
			}
		}

		// Towers from state to playerstate
		if (player_states[player_id]->towers.size() != state_towers.size()) {
			player_states[player_id]->towers.erase(
			    player_states[player_id]->towers.begin(),
			    player_states[player_id]->towers.end());
			for (int j = 0; j < state_towers.size(); ++j) {
				std::vector<PlayerTower> new_tower_vector;
				for (int k = 0; k < state_towers[j].size(); ++k) {
					// New PlayerTower
					PlayerTower new_tower;
					// Reassigns id to all the towers
					new_tower.id = state_towers[j][k]->GetActorId();

					// Init tower writables to default value
					new_tower.suicide = false;
					new_tower.upgrade_tower = false;

					// Init tower properties from state
					new_tower.hp = state_towers[j][k]->GetHp();
					if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1)
						new_tower.position = state_towers[j][k]->GetPosition();
					else {
						new_tower.position = FlipPosition(
						    map, state_towers[j][k]->GetPosition());
					}
					new_tower.level = state_towers[j][k]->GetTowerLevel();

					new_tower_vector.push_back(new_tower);
				}
				player_states[player_id]->towers.push_back(new_tower_vector);
			}
		} else {
			for (int j = 0; j < state_towers.size(); ++j) {
				for (int k = 0; k < state_towers[j].size(); ++k) {
					if (k < player_states[player_id]->towers[j].size()) {
						// Reassigns id to all the towers
						player_states[player_id]->towers[j][k].id =
						    state_towers[j][k]->GetActorId();

						// Init tower writables to default value
						player_states[player_id]->towers[j][k].suicide = false;
						player_states[player_id]->towers[j][k].upgrade_tower =
						    false;

						// Init tower properties from state
						player_states[player_id]->towers[j][k].hp =
						    state_towers[j][k]->GetHp();
						if (static_cast<PlayerId>(player_id) ==
						    PlayerId::PLAYER1)
							player_states[player_id]->towers[j][k].position =
							    state_towers[j][k]->GetPosition();
						else {
							player_states[player_id]->towers[j][k].position =
							    FlipPosition(map,
							                 state_towers[j][k]->GetPosition());
						}
						player_states[player_id]->towers[j][k].level =
						    state_towers[j][k]->GetTowerLevel();
					} else {
						// New PlayerTower
						PlayerTower new_tower;
						// Reassigns id to all the towers
						new_tower.id = state_towers[j][k]->GetActorId();

						// Init tower writables to default value
						new_tower.suicide = false;
						new_tower.upgrade_tower = false;

						// Init tower properties from state
						new_tower.hp = state_towers[j][k]->GetHp();
						if (static_cast<PlayerId>(player_id) ==
						    PlayerId::PLAYER1)
							new_tower.position =
							    state_towers[j][k]->GetPosition();
						else {
							new_tower.position = FlipPosition(
							    map, state_towers[j][k]->GetPosition());
						}
						new_tower.level = state_towers[j][k]->GetTowerLevel();

						// Pushing new Tower into the PlayerState towers.
						player_states[player_id]->towers[j].push_back(
						    new_tower);
					}
				}
				// If player_states has excess towers
				if (player_states[player_id]->towers[j].size() >
				    state_towers[j].size()) {
					player_states[player_id]->towers[j].erase(
					    player_states[player_id]->towers[j].begin() +
					        state_towers[j].size(),
					    player_states[player_id]->towers[j].end());
				}
			}
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
		if (player_id == static_cast<int>(PlayerId::PLAYER_COUNT) - 1)
			player_states[player_id]->map = std::move(player_map);
		else
			player_states[player_id]->map = player_map;
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
