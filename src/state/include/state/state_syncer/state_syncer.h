/**
 * @file state_syncer.h
 * Declarations for a state syncer
 */
#ifndef STATE_SYNCER_STATE_SYNCER_H
#define STATE_SYNCER_STATE_SYNCER_H

#include <vector>
#include "state/state.h"
#include "state/player_state.h"
#include "state/interfaces/i_state_syncer.h"
#include "state/interfaces/i_command_taker.h"
#include "state/state_export.h"

namespace state {

/**
 * Declaration for a state syncer class
 */
class STATE_EXPORT StateSyncer : public IStateSyncer {
private:

	/**
	 * Pointer to the main state
	 */
	ICommandTaker* state;

	// The functions below call corresponding action functions in State

	/**
	 * Handles soldier movement commands
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
	 * Handles enemy tower attack commands
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
	 * Handles enemy soldier attack commands
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
	 * Handles tower build commands
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
	 * Handles tower upgrade commands
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
	 * Handles tower self destruct commands
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

public:

	/**
	 * Constructor for StateSyncer class
	 */
	StateSyncer(
		ICommandTaker* state
	);

	/**
	 * Function that takes the player states and exeutes commands
	 * Also checks player moves and rejects them if invalid
	 * Calls corresponding action functions on valid moves
	 *
	 * @param[in]   player_states               list of player states to
	 *                                          read and execute moves from
	 * @param[in]   skip_player_commands_flags  if true at an index, skip that
	 *                                          player_id's turn
	 */
	void ExecutePlayerCommands(
		const std::vector<PlayerState *>& player_states,
		const std::vector<bool>& skip_player_commands_flags
	);

	/**
	 * Call updates on the main state to update state values
	 * This effectively finishes a turn and refreshes the state
	 *
	 * @return  list of player scores
	 */
	std::vector<int64_t> UpdateMainState();

	/**
	 * After the state has been updated, refresh the player states
	 * with new values, and remove the player moves
	 *
	 * @param[inout]  player_states  list of player states to update
	 */
	void UpdatePlayerStates(
		std::vector<PlayerState *>& player_states
	);

};

}

#endif