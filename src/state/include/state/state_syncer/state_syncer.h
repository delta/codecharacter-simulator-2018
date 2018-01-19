/**
 * @file state_syncer.h
 * Declarations for a state syncer
 */
#ifndef STATE_SYNCER_STATE_SYNCER_H
#define STATE_SYNCER_STATE_SYNCER_H

#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_state.h"
#include "state/interfaces/i_state_syncer.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/state_export.h"
#include <memory>
#include <vector>

namespace state {

/**
 * Declaration for a state syncer class
 */
class STATE_EXPORT StateSyncer : public IStateSyncer {
  private:
	/**
	 * Pointer to the main state
	 */
	std::unique_ptr<IState> state;

	/**
	 * Pointer to the logger instance
	 */
	logger::ILogger *logger;

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
	void MoveSoldier(PlayerId player_id, int64_t soldier_id,
	                 physics::Vector position);

	/**
	 * Handles enemy tower attack commands
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  soldier_id    soldier who is attacking
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void AttackTower(PlayerId player_id, int64_t soldier_id, int64_t tower_id);

	/**
	 * Handles enemy soldier attack commands
	 *
	 * @param[in]  player_id           player to act upon
	 * @param[in]  soldier_id          soldier who is attacking
	 * @param[in]  enemy_soldier_id    soldier who is to be attacked
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void AttackSoldier(PlayerId player_id, int64_t soldier_id,
	                   int64_t enemy_soldier_id);

	/**
	 * Handles tower build commands
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  position      position to build the tower at
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void BuildTower(PlayerId player_id, physics::Vector position);

	/**
	 * Handles tower upgrade commands
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      tower to act upon
	 *
	 * @throw      std::exception  if the operation was not possible
	 */
	void UpgradeTower(PlayerId player_id, int64_t tower_id);

	/**
	 * Handles tower self destruct commands
	 *
	 * @param[in]  player_id     player to act upon
	 * @param[in]  tower_id      opponent tower to attack
	 *
	 * @throw      std::exception if the operation was not possible
	 */
	void SuicideTower(PlayerId player_id, int64_t tower_id);

	/**
	 * Changes orientation of map such that there is a
	 * commom point of origin for all player states for all
	 * map related activities
	 *
	 * @param[in]	player_map		map to be inverted
	 *
	 */
	void FlipMap(std::vector<std::vector<PlayerMapElement>> &player_map);

	/**
	 * Returns flipped position
	 *
	 * @param[in]	map         map of state
	 * @param[in] 	position    position to be flipped
	 */
	physics::Vector FlipPosition(IMap *map, physics::Vector position);

	/**
	 * Logs the logic errors made by player
	 *
	 * @param[in]	player_id  	  player identifier
	 * @param[in]	error_code	  code of error
	 * @param[in]	message	      error message
	 *
	 */
	void LogErrors(PlayerId player_id, int64_t error_code, std::string message);

  public:
	/**
	 * Constructor for StateSyncer class
	 */
	StateSyncer(std::unique_ptr<IState> state, logger::ILogger *logger);

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
	    const std::vector<PlayerState *> &player_states,
	    const std::vector<bool> &skip_player_commands_flags) override;

	/**
	 * Call updates on the main state to update state values
	 * This effectively finishes a turn and refreshes the state
	 *
	 * @return  list of player scores
	 */
	std::vector<int64_t> UpdateMainState() override;

	/**
	 * After the state has been updated, refresh the player states
	 * with new values, and remove the player moves
	 *
	 * @param[inout]  player_states  list of player states to update
	 */
	void UpdatePlayerStates(std::vector<PlayerState *> &player_states) override;
};
}

#endif
