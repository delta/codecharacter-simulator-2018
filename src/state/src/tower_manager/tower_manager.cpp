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
	int element_size = map->GetElementSize();

	// Territory Checks
	MapElement tower_element = map->GetElementByOffset(offset);
	std::vector<bool> element_ownership = tower_element.GetOwnership();

	// Check if the location is on land
	if (tower_element.GetTerrainType() != TerrainType::LAND) {
		throw std::out_of_range("Cannot build - terrain is not land");
	}

	// Check if player owns territory
	bool this_players_territory =
	    element_ownership[static_cast<int>(this->player_id)];
	bool only_this_players_territory = true;

	// Check if no one else owns territory
	for (int i = 0; i < element_ownership.size(); ++i) {

		// Skip the own player's ownership prop when checking
		if (i == static_cast<int>(this->player_id))
			continue;

		// If someone else owns this territory, break
		if (element_ownership[i]) {
			only_this_players_territory = false;
			break;
		}
	}

	if (!(this_players_territory && only_this_players_territory)) {
		throw std::out_of_range("Invalid territory to build");
	}

	// Check if a tower already exists in the same location
	physics::Vector tower_position;

	for (int i = 0; i < towers.size(); ++i) {
		tower_position = towers[i]->GetPosition();
		if ((tower_position / element_size).floor() == offset.floor()) {
			throw std::out_of_range("Invalid location - tower already exists");
		}
	}

	// Check if the player has enough money to buy a new tower
	int64_t tower_cost = TowerManager::build_costs[0];

	if (tower_cost > money_manager->GetBalance(this->player_id)) {
		throw std::out_of_range("Tower build failed due to insufficient funds");
	}

	// Everything is good. Build the tower

	// Deduct the amount
	money_manager->Decrease(this->player_id, tower_cost);

	// Get the next ActorId
	auto new_actor_id = Actor::GetNextActorId();

	// Make the new tower
	auto new_tower = std::make_unique<Tower>(
	    new_actor_id, this->player_id, ActorType::TOWER,
	    Tower::max_hp_levels[0], Tower::max_hp_levels[0],
	    physics::Vector(element_size * offset.x + (element_size / 2),
	                    element_size * offset.y + (element_size / 2)),
	    is_base, 1);

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

void TowerManager::UpgradeTower(ActorId tower_id) {

	int64_t current_tower_index;
	bool tower_exists = false;
	for (int i = towers.size() - 1; i >= 0; --i) {
		if (towers[i]->GetActorId() == tower_id) {
			current_tower_index = i;
			tower_exists = true;
			break;
		}
	}

	// Check if tower exists
	if (!tower_exists) {
		throw std::out_of_range("Invalid tower_id, tower does not exist");
	}

	int64_t current_tower_level =
	    static_cast<int64_t>(towers[current_tower_index]->GetTowerLevel());

	// Check if the tower is upgradable
	if (current_tower_level == TowerManager::build_costs.size()) {
		throw std::out_of_range("Max level reached, upgrade not allowed");
	}

	int64_t tower_upgrade_cost = TowerManager::build_costs[current_tower_level];

	// Check if the player has sufficient balance
	int64_t current_balance = this->money_manager->GetBalance(player_id);
	if (current_balance < tower_upgrade_cost) {
		throw std::out_of_range("Insufficient balance for upgrade");
	}

	// Everything is good, upgrade the tower

	// Deduct Amount
	money_manager->Decrease(player_id, tower_upgrade_cost);

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

void TowerManager::SuicideTower(ActorId tower_id) {
	int64_t current_tower_index;
	bool tower_exists = false;
	for (int i = towers.size() - 1; i >= 0; --i) {
		if (towers[i]->GetActorId() == tower_id) {
			current_tower_index = i;
			tower_exists = true;
			break;
		}
	}
	if (!tower_exists) {
		throw std::out_of_range("Invalid tower_id, tower does not exist");
	}

	// If it's a base tower, don't kill it
	if (towers[current_tower_index]->GetIsBase()) {
		throw std::logic_error("Cannot destroy base tower");
	}

	// Destroy tower
	towers[current_tower_index]->SetHp(0);
}

void TowerManager::Update() {
	std::vector<int64_t> tower_indices_to_delete;

	// Find Dead Towers
	for (int i = 0; i < towers.size(); ++i) {
		if (towers[i]->GetHp() == 0) {
			tower_indices_to_delete.push_back(i);
		}
	}

	// Delete Dead Towers, and deallocate their territory
	for (int k = tower_indices_to_delete.size() - 1; k >= 0; --k) {
		int tower_i = tower_indices_to_delete[k];

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

		// Delete the tower from the list
		towers.erase(towers.begin() + tower_i);
	}
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
