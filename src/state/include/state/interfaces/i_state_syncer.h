/**
 * @file i_state_syncer.h
 * Interface for a state syncer
 */
#ifndef STATE_INTERFACES_I_STATE_SYNCER_H
#define STATE_INTERFACES_I_STATE_SYNCER_H

#include "state/player_state.h"
#include "state/state_export.h"
#include <vector>

namespace state {

/**
 * Interface for a state syncer
 */
class STATE_EXPORT IStateSyncer {
  public:
	/**
	 * Runs a player's commands.
	 *
	 * Skips running a player's commands if their flag is true.
	 *
	 * @param[in]  player_states               The player states
	 * @param[in]  skip_player_commands_flags  The skip player commands flags
	 *                                         indexed by player ID
	 */
	virtual void ExecutePlayerCommands(
	    const std::vector<PlayerState *> &player_states,
	    const std::vector<bool> &skip_player_commands_flags) = 0;

	/**
	 * Updates the main state
	 */
	virtual void UpdateMainState() = 0;

	/**
	 * Writes the main state back into player state copies
	 *
	 * @param      player_states  The player states
	 */
	virtual void
	UpdatePlayerStates(std::vector<PlayerState *> &player_states) = 0;

	/**
	 * Get game scores of players, indexed by player ID
	 *
	 * @return      Vector of scores
	 */
	virtual std::vector<int64_t> GetScores() = 0;

	/**
	 * Destructor
	 */
	virtual ~IStateSyncer() {}
};
}

#endif
