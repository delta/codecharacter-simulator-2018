/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {

State::State() {
	// Init None
}

State::State(std::vector<std::vector<std::unique_ptr<Soldier>>> soldiers,
             std::unique_ptr<IMap> map,
             std::unique_ptr<MoneyManager> money_manager,
             std::vector<std::unique_ptr<TowerManager>> tower_managers)
    : soldiers(std::move(soldiers)), map(std::move(map)),
      money_manager(std::move(money_manager)),
      tower_managers(std::move(tower_managers)) {}

const std::vector<std::vector<Soldier *>> State::GetAllSoldiers() {
	std::vector<std::vector<Soldier *>> ret_soldiers;
	for (int i = 0; i < soldiers.size(); ++i) {
		std::vector<Soldier *> player_soldiers;
		for (int j = 0; j < soldiers[i].size(); ++j) {
			player_soldiers.push_back(soldiers[i][j].get());
		}
		ret_soldiers.push_back(player_soldiers);
	}
	return ret_soldiers;
}

const std::vector<std::vector<Tower *>> State::GetAllTowers() {
	std::vector<std::vector<Tower *>> ret_towers;
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

const IMap *State::GetMap() { return this->map.get(); }

void State::MoveSoldier(PlayerId player_id, int64_t soldier_id,
                        physics::Vector position) {
	// TODO: Define Function
}

void State::AttackTower(PlayerId player_id, int64_t soldier_id,
                        int64_t tower_id) {
	// TODO: Define Function
}

void State::AttackSoldier(PlayerId player_id, int64_t soldier_id,
                          int64_t enemy_soldier_id) {
	// TODO: Define Function
}

void State::BuildTower(PlayerId player_id, int64_t tower_id,
                       physics::Vector position) {
	// TODO: Define Function
}

void State::UpgradeTower(PlayerId player_id, int64_t tower_id) {
	// TODO: Define Function
}

void State::SuicideTower(PlayerId player_id, int64_t tower_id) {
	// TODO: Define Function
}
}
