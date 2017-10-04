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
	std::vector<std::vector<Soldier*> > p_soldiers,
	IMap* map,
	MoneyManager* money_manager,
	std::vector<TowerManager*> p_tower_managers
	): map(map),
	money_manager(money_manager) {
	for(int i = 0; i < p_soldiers.size(); ++i) {
		for(int j = 0; j < p_soldiers[i].size(); ++j) {
			soldiers[i].emplace_back(std::move(p_soldiers[i][j]));
		}
	}

	for(int i = 0; i < p_tower_managers.size(); ++i) {
		this->tower_managers.emplace_back(std::move(p_tower_managers[i]));
	}
}

std::vector<std::vector<Soldier*> > State::GetAllSoldiers() {
	std::vector<Soldier*> player_soldiers;
	std::vector<std::vector<Soldier*> > ret_soldiers;

	for (int i = 0; i < soldiers.size(); ++i) {
		for (int j = 0; j < soldiers[i].size(); ++j) {
			player_soldiers.push_back(soldiers[i][j].get());
		}
		ret_soldiers.push_back(player_soldiers);
		player_soldiers.clear();
	}

	return ret_soldiers;
}

std::vector<std::vector<Tower*> > State::GetAllTowers() {
	std::vector<std::vector<Tower*> > ret_towers;
	for (int i = 0; i < this->tower_managers.size(); ++i) {
		ret_towers.push_back(this->tower_managers[i]->GetTowers());
	}
	return ret_towers;
}

std::vector<int64_t> State::GetMoney() {
	std::vector<int64_t> ret_balance;
	int player_count = static_cast<int>(PlayerId::PLAYER_COUNT);
	for (int i = 0; i < player_count; ++i) {
		ret_balance.push_back(money_manager->GetBalance(PlayerId(i)));
	}
	return ret_balance;
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
