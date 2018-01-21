/**
 * @file tower_manager.cpp
 * Defines the methods of the TowerManager class
 */

#include "state/tower_manager/tower_manager.h"
#include "physics/vector.h"
#include <algorithm>
#include <cmath>
#include <memory>

namespace state {

TowerManager::TowerManager() {
	// Init None
}

TowerManager::TowerManager(std::vector<std::unique_ptr<Tower>> towers,
                           PlayerId player_id, MoneyManager *money_manager,
                           IMap *map)
    : towers(std::move(towers)), player_id(player_id),
      money_manager(money_manager), map(map),
      territory_refs(std::vector<std::vector<int64_t>>(
          map->GetSize(), std::vector<int64_t>(map->GetSize(), 0))) {

	// Set default territory based on initial towers
	for (auto &tower : towers) {
		auto bounds = CalculateBounds(tower.get());

		for (int i = bounds[0].x; i <= bounds[1].x; ++i) {
			for (int j = bounds[0].y; j <= bounds[1].y; ++j) {
				territory_refs[i][j]++;
				map->GetElementByOffset(physics::Vector(i, j))
				    .SetOwnership(player_id, true);
			}
		}
	}
}

std::vector<physics::Vector> TowerManager::CalculateBounds(Tower *tower) {
	int map_size = map->GetSize();
	int element_size = map->GetElementSize();

	physics::Vector tower_position = tower->GetPosition();
	physics::Vector tower_offset = (tower_position / element_size).floor();

	int64_t range = TowerManager::tower_ranges[tower->GetTowerLevel() - 1];

	int64_t lower_x = std::max((int)(tower_offset.x - range), 0);
	int64_t upper_x = std::min((int)(tower_offset.x + range), map_size - 1);
	int64_t lower_y = std::max((int)(tower_offset.y - range), 0);
	int64_t upper_y = std::min((int)(tower_offset.y + range), map_size - 1);

	std::vector<physics::Vector> result = {physics::Vector(lower_x, lower_y),
	                                       physics::Vector(upper_x, upper_y)};

	return result;
}

void TowerManager::BuildTower(physics::Vector offset, bool is_base) {
	this->towers_to_build_offsets.push(offset);
}

void TowerManager::UpgradeTower(ActorId tower_id) {
	this->towers_to_upgrade.push(tower_id);
}

void TowerManager::SuicideTower(ActorId tower_id) {
	this->towers_to_suicide.push(tower_id);
	int64_t current_tower_index;
}

void TowerManager::HandleTowerBuildUpdates() {
	auto element_size = this->map->GetElementSize();

	// Iterate through list of offsets to build towers at
	while (!this->towers_to_build_offsets.empty()) {
		// Fetch the offset to build the tower at
		const auto build_offset = this->towers_to_build_offsets.front();
		this->towers_to_build_offsets.pop();

		// Deduct the amount
		money_manager->Decrease(this->player_id, this->build_costs[0]);

		// Get the next ActorId
		auto new_actor_id = Actor::GetNextActorId();

		// Make the new tower
		auto new_tower = std::make_unique<Tower>(
		    new_actor_id, this->player_id, ActorType::TOWER,
		    Tower::max_hp_levels[0], Tower::max_hp_levels[0],
		    physics::Vector(element_size * build_offset.x + (element_size / 2),
		                    element_size * build_offset.y + (element_size / 2)),
		    false, 1);

		auto tower_ptr = new_tower.get();

		// Add the new tower
		this->towers.push_back(std::move(new_tower));

		// Mark the new territory
		auto bounds = CalculateBounds(tower_ptr);

		for (int i = bounds[0].x; i <= bounds[1].x; ++i) {
			for (int j = bounds[0].y; j <= bounds[1].y; ++j) {
				territory_refs[i][j]++;
				map->GetElementByOffset(physics::Vector(i, j))
				    .SetOwnership(player_id, true);
			}
		}
	}
}

void TowerManager::HandleTowerUpgradeUpdates() {
	// Iterate through list of tower IDs to upgrade
	while (!this->towers_to_upgrade.empty()) {
		// Get ID of tower to upgrade
		const auto tower_id = this->towers_to_upgrade.front();
		this->towers_to_upgrade.pop();

		// Get the index of the tower in the list of towers
		int64_t current_tower_index = -1;
		for (int i = towers.size() - 1; i >= 0; --i) {
			if (towers[i]->GetActorId() == tower_id) {
				current_tower_index = i;
				break;
			}
		}

		// Check if tower exists
		if (current_tower_index == -1) {
			throw std::out_of_range("Cannot upgrade non-existent tower");
		}

		int64_t current_tower_level =
		    static_cast<int64_t>(towers[current_tower_index]->GetTowerLevel());

		// Check if the tower is upgradable
		if (current_tower_level == TowerManager::build_costs.size()) {
			throw std::out_of_range("Max level reached, upgrade not allowed");
		}

		// Deduct Amount
		money_manager->Decrease(player_id,
		                        this->build_costs[current_tower_level]);

		// Save old territory bounds
		auto old_bounds = CalculateBounds(towers[current_tower_index].get());

		// Upgrade the tower
		towers[current_tower_index]->Upgrade();

		// Set new territory
		auto new_bounds = CalculateBounds(towers[current_tower_index].get());

		for (int i = new_bounds[0].x; i <= new_bounds[1].x; ++i) {
			for (int j = new_bounds[0].y; j <= new_bounds[1].y; ++j) {
				// Avoid setting territory that was already set pre-upgrade
				if (i >= old_bounds[0].x && i <= old_bounds[1].x &&
				    j >= old_bounds[0].y && j <= old_bounds[1].y)
					continue;
				territory_refs[i][j]++;
				map->GetElementByOffset(physics::Vector(i, j))
				    .SetOwnership(player_id, true);
			}
		}
	}
}

void TowerManager::HandleTowerSuicideUpdates() {
	// Iterate through list of tower IDs to suicide
	while (!this->towers_to_suicide.empty()) {
		// Get ID of tower to suicide
		const auto tower_id = this->towers_to_suicide.front();
		this->towers_to_suicide.pop();

		// Get the index of the tower in the list of towers
		int64_t current_tower_index = -1;
		for (int i = towers.size() - 1; i >= 0; --i) {
			if (towers[i]->GetActorId() == tower_id) {
				current_tower_index = i;
				break;
			}
		}

		// Check if tower exists
		if (current_tower_index == -1) {
			throw std::out_of_range("Cannot suicide non-existent tower");
		}

		// Destroy tower
		towers[current_tower_index]->SetHp(0);

		// Add reward amount
		money_manager->RewardSuicide(towers[current_tower_index].get());
	}
}

void TowerManager::HandleTowerDeathUpdates() {
	std::vector<int64_t> tower_indices_to_delete;

	// Delete towers two turns old
	this->towers_to_delete[0].clear();
	// Shift towers that were one turn old to the list for deletion next turn
	this->towers_to_delete[0] = std::move(this->towers_to_delete[1]);

	// Find Dead Towers
	for (int i = 0; i < towers.size(); ++i) {
		if (towers[i]->GetHp() == 0) {
			tower_indices_to_delete.push_back(i);
		}
	}

	// Delete Dead Towers, and deallocate their territory
	for (auto tower_index = tower_indices_to_delete.rbegin();
	     tower_index != tower_indices_to_delete.rend(); ++tower_index) {
		int tower_i = *tower_index;

		auto bounds = CalculateBounds(towers[tower_i].get());

		// If territory references hits zero, deallocate the territory
		for (int i = bounds[0].x; i <= bounds[1].x; ++i) {
			for (int j = bounds[0].y; j <= bounds[1].y; ++j) {
				territory_refs[i][j]--;
				if (territory_refs[i][j] == 0) {
					map->GetElementByOffset(physics::Vector(i, j))
					    .SetOwnership(player_id, false);
				}
			}
		}

		// Move the tower from main list to list for deletion, will be removed
		// fully after two turns
		this->towers_to_delete[1].push_back(std::move(towers[tower_i]));

		// Delete the tower from the main list
		towers.erase(towers.begin() + tower_i);
	}
}

void TowerManager::Update() {
	this->HandleTowerBuildUpdates();
	this->HandleTowerUpgradeUpdates();
	this->HandleTowerSuicideUpdates();
	this->HandleTowerDeathUpdates();
}

Tower *TowerManager::GetTowerById(ActorId tower_id) {
	int64_t tower_index;
	bool tower_exists = false;
	for (int i = towers.size() - 1; i >= 0; --i) {
		if (towers[i]->GetActorId() == tower_id) {
			tower_index = i;
			tower_exists = true;
			break;
		}
	}
	if (!tower_exists) {
		throw std::out_of_range("Invalid tower_id, tower does not exist");
	}
	return towers[tower_index].get();
}

std::vector<Tower *> TowerManager::GetTowers() {
	std::vector<Tower *> ret_towers;
	for (int i = 0; i < towers.size(); ++i) {
		ret_towers.push_back(towers[i].get());
	}
	return ret_towers;
}
}
