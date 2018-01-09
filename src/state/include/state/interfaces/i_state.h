/**
 * @file i_state.h
 * Interface for state methods
 */
#ifndef STATE_INTERFACES_I_STATE_H
#define STATE_INTERFACES_I_STATE_H

#include "state/actor/soldier.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/interfaces/i_map.h"
#include "state/map/map_element.h"
#include "state/state_export.h"
#include <vector>

namespace state {

/**
 * Declarations for the IState interface, which declares methods to get
 * state values and modify the state
 */
class STATE_EXPORT IState : public IUpdatable {
  public:
	/**
	 * Get all soldiers, indexed by PlayerId
	 *
	 * @return      Vector of all soldiers in state
	 */
	virtual const std::vector<std::vector<Soldier *>> GetAllSoldiers() = 0;

	/**
	 * Get all towers, indexed by PlayerId
	 *
	 * @return      Vector of all towers in state
	 */
	virtual const std::vector<std::vector<Tower *>> GetAllTowers() = 0;

	/**
	 * Get money for both players
	 *
	 * @return      Vector of integers with players' money
	 */
	virtual std::vector<int64_t> GetMoney() = 0;

	/**
	 * Get the map
	 *
	 * @return      Grid of MapElements
	 */
	virtual IMap *GetMap() = 0;

	/**
	 * Handles soldier movement
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier to act upon
	 * @param[in]  position      new position to move the soldier to
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void MoveSoldier(PlayerId player_id, int64_t soldier_id,
	                         physics::Vector position) = 0;

	/**
	 * Handles attacks on enemy units
	 *
	 * @param[in]  player_id           player to act upon
	 * @param[in]  soldier_id          soldier who is attacking
	 * @param[in]  enemy_actor_id      opponent actor to attack
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void AttackActor(PlayerId player_id, int64_t soldier_id,
	                         int64_t enemy_actor_id) = 0;

	/**
	 * Handles tower builds
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  position      position to build the tower at
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void BuildTower(PlayerId player_id, physics::Vector position) = 0;

	/**
	 * Handles tower upgrades
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      tower to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void UpgradeTower(PlayerId player_id, int64_t tower_id) = 0;

	/**
	 * Handles tower self destructs
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception if the operation was not possible
	 */
	virtual void SuicideTower(PlayerId player_id, int64_t tower_id) = 0;
};
}

#endif
