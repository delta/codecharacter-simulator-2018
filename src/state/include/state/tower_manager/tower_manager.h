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

  public:
	/**
	 * Constructor for TowerManager class
	 */
	TowerManager();

	TowerManager(std::vector<Tower *> towers, PlayerId player_id,
	             MoneyManager *money_manager, IMap *map);

	/**
	 * Amount of money necessary to upgrade tower to next level
	 * First element represents initial build cost
	 */
	static std::vector<int64_t> build_costs;

	/**
	 * Tower Max HP levels corresponding to each upgrade level
	 * First element corresponds to base map hp
	 */
	static std::vector<int64_t> max_hp_levels;

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
