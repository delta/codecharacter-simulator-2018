/**
 * @file tower_manager_static_init.cpp
 * Sets the constant static members of the TowerManager class
 */

#include "physics/vector.h"
#include "state/tower_manager/tower_manager.h"

namespace state {

std::vector<int64_t> TowerManager::build_costs = {500, 700, 900};
std::vector<int64_t> TowerManager::max_hp_levels = {100, 200, 350};
std::vector<int64_t> TowerManager::tower_ranges = {1, 2, 3};
}
