/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#ifndef STATE_STATE_H
#define STATE_STATE_H

#include <iostream>
#include <vector>
#include <memory>
#include "physics/vector.h"
#include "state/actor/soldier.h"
#include "state/actor/tower.h"
#include "state/map/interfaces/i_map.h"
#include "state/interfaces/i_command_taker.h"
#include "state/money_manager/money_manager.h"
#include "state/tower_manager/tower_manager.h"
#include "state/state_export.h"

namespace state {

class STATE_EXPORT State : public ICommandTaker {
protected:
	/**
	 * List of all soldiers in the game, indexed by PlayerId
	 */
	std::vector<std::vector<std::unique_ptr<Soldier> > > soldiers;

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
	std::vector<std::unique_ptr<TowerManager> > tower_managers;

public:
	State();

	State& operator=(const State& other) = delete;

	State(const State& other) = delete;

	State(
		std::vector<std::vector<Soldier*> > soldiers,
		IMap* map,
		MoneyManager* money_manager,
		std::vector<TowerManager*> tower_managers
	);

	~State() = default;

	/**
	 * Get all soldiers, indexed by PlayerId
	 *
	 * @return      Vector of all soldiers in state
	 */
	std::vector<std::vector<Soldier*> > GetAllSoldiers();

	/**
	 * Get all towers, indexed by PlayerId
	 *
	 * @return      Vector of all towers in state
	 */
	std::vector<std::vector<Tower*> > GetAllTowers();

	/**
	 * Get money for both players
	 *
	 * @return      Vector of integers with players' money
	 */
	std::vector<int64_t> GetMoney();

	/**
	 * Handles soldier movement
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier to act upon
	 * @param[in]  position      new position to move the soldier to
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void MoveSoldier(
		PlayerId player_id,
		int64_t soldier_id,
		physics::Vector position
	);

	/**
	 * Handles enemy tower attacks
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier who is attacking
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void AttackTower(
		PlayerId player_id,
		int64_t soldier_id,
		int64_t tower_id
	);

	/**
	 * Handles enemy soldier attacks
	 *
	 * @param[in]  player_id           player to act upon
	 * @param[in]  soldier_id          soldier who is attacking
	 * @param[in]  enemy_soldier_id    soldier who is to be attacked
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void AttackSoldier(
		PlayerId player_id,
		int64_t soldier_id,
		int64_t enemy_soldier_id
	);

	/**
	 * Handles tower builds
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      tower to act upon
	 * @param[in]  position      position to build the tower at
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void BuildTower(
		PlayerId player_id,
		int64_t tower_id,
		physics::Vector position
	);

	/**
	 * Handles tower upgrades
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      tower to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void UpgradeTower(
		PlayerId player_id,
		int64_t tower_id
	);

	/**
	 * Handles tower self destructs
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception if the operation was not possible
	 */
	void SuicideTower(
		PlayerId player_id,
		int64_t tower_id
	);
};

}

#endif
