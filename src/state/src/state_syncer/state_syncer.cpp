/**
 * @file state_syncer.cpp
 * Definitions for a state syncer
 */

#include "state/state_syncer/state_syncer.h"

namespace state {

StateSyncer::StateSyncer(IState *state) : state(state) {}

void StateSyncer::ExecutePlayerCommands(
    const std::vector<PlayerState *> &player_states,
    const std::vector<bool> &skip_player_commands_flags) {
	// TODO: Define function
}

std::vector<int64_t> StateSyncer::UpdateMainState() {
	// TODO: Define function
	return std::vector<int64_t>(1, 1);
}

void StateSyncer::UpdatePlayerStates(
    std::vector<PlayerState *> &player_states) {
	// TODO: Define function
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

void StateSyncer::BuildTower(PlayerId player_id, int64_t tower_id,
                             physics::Vector position) {
	// TODO: Define function
}

void StateSyncer::UpgradeTower(PlayerId player_id, int64_t tower_id) {
	// TODO: Define function
}

void StateSyncer::SuicideTower(PlayerId player_id, int64_t tower_id) {
	// TODO: Define function
}
}
