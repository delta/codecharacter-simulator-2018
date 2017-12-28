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

TowerManager::TowerManager(std::vector<Tower *> p_towers, PlayerId player_id,
                           MoneyManager *money_manager, IMap *map)
    : player_id(player_id), money_manager(money_manager), map(map),
      territory_refs(std::vector<std::vector<int64_t>>(
          map->GetSize(), std::vector<int64_t>(map->GetSize(), 0))) {

	for (int i = 0; i < p_towers.size(); ++i) {
		towers.emplace_back(std::move(p_towers[i]));
	}
}

void TowerManager::BuildTower(physics::Vector offset, bool is_base) {
	int64_t map_size = map->GetSize();
	int64_t element_size = map->GetElementSize();

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
		if (floor(tower_position.x / element_size) == floor(offset.x) &&
		    floor(tower_position.y / element_size) == floor(offset.y)) {

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
	int64_t new_actor_id = Actor::GetNextActorId();

	// Add the new tower
	this->towers.push_back(std::make_unique<Tower>(
	    new_actor_id, this->player_id, ActorType::TOWER,
	    Tower::max_hp_levels[0], Tower::max_hp_levels[0],
	    physics::Vector(element_size * offset.x + (element_size / 2),
	                    element_size * offset.y + (element_size / 2)),
	    is_base, 1));

	// Mark the new territory controlled by the tower
	int64_t range = TowerManager::tower_ranges[0];

	int64_t lower_x = std::max((int)(offset.x - range), (int)0);
	int64_t upper_x = std::min((int)(offset.x + range), (int)map_size - 1);
	int64_t lower_y = std::max((int)(offset.y - range), (int)0);
	int64_t upper_y = std::min((int)(offset.y + range), (int)map_size - 1);

	for (int i = lower_x; i <= upper_x; ++i) {
		for (int j = lower_y; j <= upper_y; ++j) {
			territory_refs[i][j]++;
			map->GetElementByOffset(physics::Vector(i, j))
			    .SetOwnership(player_id, true);
		}
	}
}

void TowerManager::UpgradeTower(state::ActorId tower_id) {

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

	int64_t map_size = map->GetSize();
	int64_t element_size = map->GetElementSize();

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

	// Calculate new territory bounds
	physics::Vector tower_position = towers[current_tower_index]->GetPosition();
	physics::Vector tower_offset =
	    physics::Vector(floor(tower_position.x / element_size),
	                    floor(tower_position.y / element_size));

	int64_t range = TowerManager::tower_ranges[current_tower_level];

	int64_t lower_x = std::max((int)(tower_offset.x - range), (int)0);
	int64_t upper_x =
	    std::min((int)(tower_offset.x + range), (int)map_size - 1);
	int64_t lower_y = std::max((int)(tower_offset.y - range), (int)0);
	int64_t upper_y =
	    std::min((int)(tower_offset.y + range), (int)map_size - 1);

	// Set new territory
	for (int i = lower_x; i <= upper_x; ++i) {
		for (int j = lower_y; j <= upper_y; ++j) {
			territory_refs[i][j]++;
			map->GetElementByOffset(physics::Vector(i, j))
			    .SetOwnership(player_id, true);
		}
	}

	towers[current_tower_index]->Upgrade();
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
		return;
	}

	// Destroy tower
	towers[current_tower_index]->SetHp(0);
}

void TowerManager::Update() {
	int64_t map_size = map->GetSize();
	int64_t element_size = map->GetElementSize();

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

		// Find territory
		physics::Vector tower_position = towers[tower_i]->GetPosition();
		physics::Vector tower_offset =
		    physics::Vector(floor(tower_position.x / element_size),
		                    floor(tower_position.y / element_size));

		int64_t range =
		    TowerManager::tower_ranges[towers[tower_i]->GetTowerLevel() - 1];

		int64_t lower_x = std::max((int)(tower_offset.x - range), (int)0);
		int64_t upper_x =
		    std::min((int)(tower_offset.x + range), (int)map_size - 1);
		int64_t lower_y = std::max((int)(tower_offset.y - range), (int)0);
		int64_t upper_y =
		    std::min((int)(tower_offset.y + range), (int)map_size - 1);

		// If territory references hits zero, deallocate the territory
		for (int i = lower_x; i <= upper_x; ++i) {
			for (int j = lower_y; j <= upper_y; ++j) {
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
