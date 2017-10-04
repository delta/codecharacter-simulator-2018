/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {

State::State() {
	// Init None
}

State::State(
	std::vector<Soldier*> p_soldiers,
	IMap* map,
	MoneyManager money_manager
	): map(map),
	money_manager(money_manager) {
	for(int i = 0; i < p_soldiers.size(); ++i) {
		soldiers.emplace_back(std::move(p_soldiers[i]));
	}
}

std::vector<Soldier*> State::GetAllSoldiers() {
	std::vector<Soldier*> ret_soldiers;
	for(int i = 0; i < soldiers.size(); ++i) {
		ret_soldiers.push_back(soldiers[i].get());
	}
	return ret_soldiers;
}

void State::MoveSoldier(
	PlayerId player_id,
	int64_t soldier_id,
	physics::Vector position
) {
	// TODO: Define Function
}

void State::AttackTower(
	PlayerId player_id,
	int64_t soldier_id,
	int64_t tower_id
) {
	// TODO: Define Function
}

void State::AttackSoldier(
	PlayerId player_id,
	int64_t soldier_id,
	int64_t enemy_soldier_id
) {
	// TODO: Define Function
}

void State::BuildTower(
	PlayerId player_id,
	int64_t tower_id,
	physics::Vector position
) {
	// TODO: Define Function
}

void State::UpgradeTower(
	PlayerId player_id,
	int64_t tower_id
) {
	// TODO: Define Function
}

void State::SuicideTower(
	PlayerId player_id,
	int64_t tower_id
) {
	// TODO: Define Function
}

}
