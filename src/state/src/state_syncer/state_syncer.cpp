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
	// TODO: Define function
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
	// TODO: Define function
}

void StateSyncer::AttackTower(PlayerId player_id, int64_t soldier_id,
                              int64_t tower_id) {
	// TODO: Define function
}

void StateSyncer::AttackSoldier(PlayerId player_id, int64_t soldier_id,
                                int64_t enemy_soldier_id) {
	// TODO: Define function
}

void StateSyncer::BuildTower(PlayerId player_id, physics::Vector position) {
	// TODO: Define function
}

void StateSyncer::UpgradeTower(PlayerId player_id, int64_t tower_id) {
	// TODO: Define function
}

void StateSyncer::SuicideTower(PlayerId player_id, int64_t tower_id) {
	// TODO: Define function
}
}
