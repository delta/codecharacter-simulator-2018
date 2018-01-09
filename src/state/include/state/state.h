/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#ifndef STATE_STATE_H
#define STATE_STATE_H

#include "physics/vector.h"
#include "state/actor/soldier.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_path_planner.h"
#include "state/interfaces/i_state.h"
#include "state/map/interfaces/i_map.h"
#include "state/map/map_element.h"
#include "state/money_manager/money_manager.h"
#include "state/state_export.h"
#include "state/tower_manager/tower_manager.h"
#include <iostream>
#include <memory>
#include <vector>

namespace state {

class STATE_EXPORT State : public IState {
  protected:
	/**
	 * List of all soldiers in the game, indexed by PlayerId
	 * Soldiers will always be the first set of actors, with actor_id in order
	 * starting from 0
	 */
	std::vector<std::vector<std::unique_ptr<Soldier>>> soldiers;

	/**
	 * Map of the game
	 */
	std::unique_ptr<IMap> map;

	/**
	 * MoneyManager object to handle player currency
	 */
	std::unique_ptr<MoneyManager> money_manager;

	/**
	 * TowerManager objects to handle game Towers, indexed by PlayerId
	 */
	std::vector<std::unique_ptr<TowerManager>> tower_managers;

	/**
	 * PathPlanner to perform movement and map calculations
	 */
	std::unique_ptr<IPathPlanner> path_planner;

	/**
	 * Helper function returns a pointer to a soldier
	 * PlayerId helps identify the right soldier vector faster
	 *
	 * @param[in]  actor_id
	 * @param[in]  player_id
	 *
	 * @return     Soldier* if actor exists
	 * @throw      out_of_range if soldier does not exist
	 */
	Soldier *GetSoldierById(ActorId actor_id, PlayerId player_id);

  public:
	/**
	 * Constructors for State
	 */
	State(std::vector<std::vector<std::unique_ptr<Soldier>>> soldiers,
	      std::unique_ptr<IMap> map,
	      std::unique_ptr<MoneyManager> money_manager,
	      std::vector<std::unique_ptr<TowerManager>> tower_managers,
	      std::unique_ptr<IPathPlanner> path_planner);

	State();

	// Explicitly delete copy constructors to State
	State &operator=(const State &other) = delete;
	State(const State &other) = delete;

	~State() = default;

	/**
	 * @see IIState#GetAllSoldiers
	 */
	const std::vector<std::vector<Soldier *>> GetAllSoldiers() override;

	/**
	 * @see IState#GetAllTowers
	 */
	const std::vector<std::vector<Tower *>> GetAllTowers() override;

	/**
	 * @see IState#GetMoney
	 */
	std::vector<int64_t> GetMoney() override;

	/**
	 * @see IState#GetMap
	 */
	IMap *GetMap() override;

	/**
	 * @see IState#MoveSoldier
	 */
	void MoveSoldier(PlayerId player_id, int64_t soldier_id,
	                 physics::Vector position) override;

	/**
	 * @see IState#AttackActor
	 */
	void AttackActor(PlayerId player_id, int64_t soldier_id,
	                 int64_t enemy_actor_id) override;

	/**
	 * @see IState#BuildTower
	 */
	void BuildTower(PlayerId player_id, physics::Vector position) override;

	/**
	 * @see IState#UpgradeTower
	 */
	void UpgradeTower(PlayerId player_id, int64_t tower_id) override;

	/**
	 * @see IState#SuicideTower
	 */
	void SuicideTower(PlayerId player_id, int64_t tower_id) override;

	/**
	 * State update function. Calls update functions of members
	 */
	void Update() override;
};
}

#endif
