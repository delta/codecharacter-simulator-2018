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
             std::vector<std::unique_ptr<TowerManager>> tower_managers,
             std::unique_ptr<IPathPlanner> path_planner)
    : soldiers(std::move(soldiers)), map(std::move(map)),
      money_manager(std::move(money_manager)),
      tower_managers(std::move(tower_managers)),
      path_planner(std::move(path_planner)) {}

Soldier *State::GetSoldierById(ActorId actor_id, PlayerId player_id) {
	int soldiers_per_team = soldiers[0].size();

	if (actor_id < 0) {
		throw std::out_of_range("Invalid actor_id, cannot be negative");
	}

	if (actor_id > (int)PlayerId::PLAYER_COUNT * soldiers_per_team - 1) {
		throw std::out_of_range("Invalid actor_id, no such soldier");
	}

	// Soldiers are in ascending order by actor_id. Directly get index
	int index_of_soldier = actor_id - soldiers_per_team * (int)player_id;

	return soldiers[(int)player_id][index_of_soldier].get();
}

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

IMap *State::GetMap() { return this->map.get(); }

std::vector<int64_t> State::GetScores() {
	int num_players = (int)PlayerId::PLAYER_COUNT;
	std::vector<int64_t> scores(num_players, 0);

	for (int i = 0; i < this->map->GetSize(); ++i) {
		for (int j = 0; j < this->map->GetSize(); ++j) {
			auto elt = this->map->GetElementByOffset(physics::Vector(i, j));
			for (int player_id = 0; player_id < num_players; ++player_id) {
				if (elt.GetOwnership()[player_id]) {
					scores[player_id]++;
				}
			}
		}
	}

	return scores;
}

void State::MoveSoldier(PlayerId player_id, int64_t soldier_id,
                        physics::Vector position) {
	auto soldier = GetSoldierById(soldier_id, player_id);
	soldier->Move(position);
}

void State::AttackActor(PlayerId player_id, int64_t soldier_id,
                        int64_t actor_id) {
	auto soldier = GetSoldierById(soldier_id, player_id);
	int num_players = (int)PlayerId::PLAYER_COUNT;
	Actor *target;

	// Determine if soldier or tower
	// It's a soldier if actor_id is within number of soldiers-1
	if (actor_id < num_players * soldiers[0].size()) {
		target = GetSoldierById(
		    actor_id,
		    static_cast<PlayerId>(((int)player_id + 1) % num_players));
	} else {
		target =
		    tower_managers[((int)player_id + 1) % num_players]->GetTowerById(
		        actor_id);
	}

	soldier->Attack(target);
}

void State::BuildTower(PlayerId player_id, physics::Vector position) {
	tower_managers[(int)player_id]->BuildTower(position);
}

void State::UpgradeTower(PlayerId player_id, int64_t tower_id) {
	tower_managers[(int)player_id]->UpgradeTower(tower_id);
}

void State::SuicideTower(PlayerId player_id, int64_t tower_id) {
	tower_managers[(int)player_id]->SuicideTower(tower_id);
}

void State::Update() {
	for (auto &tower_manager : this->tower_managers) {
		tower_manager->Update();
	}

	for (auto &player_soldiers : this->soldiers) {
		for (auto &soldier : player_soldiers) {
			soldier->Update();
		}
	}

	for (auto &player_soldiers : this->soldiers) {
		for (auto &soldier : player_soldiers) {
			soldier->LateUpdate();
		}
	}
}
}
