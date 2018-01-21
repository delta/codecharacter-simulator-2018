/**
 * @file tower_manager.h
 * Declares TowerManager class which handles tower mechanics
 */

#ifndef STATE_TOWER_MANAGER_TOWER_MANAGER_H
#define STATE_TOWER_MANAGER_TOWER_MANAGER_H

#include "physics/vector.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/map/map_element.h"
#include "state/money_manager/money_manager.h"
#include "state/state_export.h"
#include "state/utilities.h"
#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

namespace state {
/**
 * TowerManager class
 */
class STATE_EXPORT TowerManager : public IUpdatable {
  private:
	/**
	 * List of towers managed by this class
	 */
	std::vector<std::unique_ptr<Tower>> towers;

	/**
	 * Denotes which player this tower manager corresponds to
	 */
	PlayerId player_id;

	/**
	 * Pointer to Money Manager
	 */
	MoneyManager *money_manager;

	/**
	 * Pointer to the main map for updating territory information
	 */
	IMap *map;

	/**
	 * List of offsets to build towers at
	 *
	 * Added to by BuildTower and consumed in the Update method
	 */
	std::queue<physics::Vector> towers_to_build_offsets;

	/**
	 * List of tower IDs to upgrade to the next level
	 *
	 * Added to by UpgradeTower and consumed in the Update method
	 */
	std::queue<ActorId> towers_to_upgrade;

	/**
	 * List of tower IDs that are going to suicide
	 *
	 * Added to by SuicideTower and consumed in the Update method
	 */
	std::queue<ActorId> towers_to_suicide;

	/**
	 * Towers are stored for two extra turns before removing them from the
	 * manager. This list is populated every turn. First element is the list of
	 * towers two turns old, second element is the list of towers one turn old.
	 */
	std::array<std::vector<std::unique_ptr<Tower>>, 2> towers_to_delete;

	/**
	 * 2D grid of reference counts of the number of towers of this player that
	 * directly control particular territory. 0 by default for neutral.
	 * For ex. if three towers' ranges overlap on (3, 4), then
	 * territory_refs[3][4] would be 3.
	 * Useful when deallocating territory that is owned by multiple towers
	 */
	std::vector<std::vector<int64_t>> territory_refs;

	/**
	 * Utility function that when given a tower, returns its range on the map
	 * as a vector containing the lower bound and the upper bound
	 *
	 * @param[in]   Pointer to Tower
	 *
	 * @return   vector of physics::Vectors - [lower_bound, upper_bound]
	 */
	std::vector<physics::Vector> CalculateBounds(Tower *tower);

	/**
	 * Consumes towers_to_build_offsets and builds towers at those offsets
	 *
	 * @throw std::out_of_range If tower offset is not within the map
	 */
	void HandleTowerBuildUpdates();

	/**
	 * Consumes towers_to_upgrade and upgrades towers with the given IDs
	 *
	 * @throw std::out_of_range If an ID was invalid or tower is already at max
	 *                          level
	 */
	void HandleTowerUpgradeUpdates();

	/**
	 * Consumes towers_to_suicide and suicides towers with the given IDs
	 *
	 * @throw std::out_of_range If an ID was invalid
	 */
	void HandleTowerSuicideUpdates();

	/**
	 * Manages updating of the towers_to_delete list
	 */
	void HandleTowerDeathUpdates();

  public:
	/**
	 * Constructor for TowerManager class
	 */
	TowerManager();

	TowerManager(std::vector<std::unique_ptr<Tower>> towers, PlayerId player_id,
	             MoneyManager *money_manager, IMap *map);

	/**
	 * Amount of money necessary to upgrade tower to next level
	 * First element represents initial build cost
	 */
	static std::vector<int64_t> build_costs;

	/**
	 * Number of units of range each upgrade level corresponds to
	 * First element has the default tower range
	 */
	static std::vector<int64_t> tower_ranges;

	/**
	 * Method that attempts to upgrade the tower.
	 *
	 * @param[in]  tower_id The tower identifier
	 *
	 * @throw      std::out_of_range If upgrade cannot be performed due to
	 *                               bad tower_id, max level, or insufficient
	 * money
	 */
	void UpgradeTower(ActorId tower_id);

	/**
	 * Destroy the specified tower
	 *
	 * @param[in]  tower_id The tower identifier
	 *
	 * @throw      std::out_of_range If tower_id is invalid
	 */
	void SuicideTower(ActorId tower_id);

	/**
	 * Build a new tower at the given offset
	 *
	 * @param[in]  offset   The offset
	 * @param[in]  is_base  bool is_base makes the tower invincible when set to
	 * true
	 *                      set to false by default
	 *
	 * @throw      std::out_of_range If the tower cannot be built due to bad
	 * terrain,
	 *                               ownership, position, or insufficient
	 * balance
	 */
	void BuildTower(physics::Vector offset, bool is_base = false);

	/**
	 * Get a tower given its ActorID
	 *
	 * @param[in]  tower_id  The tower identifier
	 *
	 * @return     Pointer to tower
	 *
	 * @throw      std::out_of_range If the tower_id is invalid
	 */
	Tower *GetTowerById(ActorId tower_id);

	/**
	 * Get all the towers currently owned by this manager
	 *
	 * @return     List of tower pointers
	 */
	std::vector<Tower *> GetTowers();

	/**
	 * Update function that sets tower statuses
	 */
	void Update() override;
};
}

#endif
