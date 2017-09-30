/**
 * @file i_command_taker.h
 * Interface that takes player command actions
 */
#ifndef STATE_INTERFACES_I_COMMAND_TAKER_H
#define STATE_INTERFACES_I_COMMAND_TAKER_H

#include <vector>
#include <cmath>
#include "physics/vector.h"
#include "state/state_export.h"

namespace state {

class STATE_EXPORT ICommandTaker {
public:

	/**
	 * Handles soldier movement
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier to act upon
	 * @param[in]  position      new position to move the soldier to
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void MoveSoldier(
		PlayerId player_id,
		int64_t soldier_id,
		physics::Vector position
	) = 0;

	/**
	 * Handles enemy tower attacks
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier who is attacking
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void RazeTower(
		PlayerId player_id,
		int64_t soldier_id,
		int64_t tower_id
	) = 0;

	/**
	 * Handles enemy soldier attacks
	 *
	 * @param[in]  player_id           player to act upon
	 * @param[in]  soldier_id          soldier who is attacking
	 * @param[in]  enemy_soldier_id    soldier who is to be attacked
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void AttackSoldier(
		PlayerId player_id,
		int64_t soldier_id,
		int64_t enemy_soldier_id
	) = 0;

	/**
	 * Handles tower builds
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      tower to act upon
	 * @param[in]  position      position to build the tower at
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void BuildTower(
		PlayerId player_id,
		int64_t tower_id,
		physics::Vector position
	) = 0;

	/**
	 * Handles tower upgrades
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      tower to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	virtual void UpgradeTower(
		PlayerId player_id,
		int64_t tower_id
	) = 0;

	/**
	 * Handles tower self destructs
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception if the operation was not possible
	 */
	virtual void SuicideTower(
		PlayerId player_id,
		int64_t tower_id
	) = 0;

};

}

#endif
