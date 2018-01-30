/**
 * @file i_player_code.h
 * Interface for the player AI
 */

#ifndef PLAYER_WRAPPER_INTERFACES_I_PLAYER_CODE_H
#define PLAYER_WRAPPER_INTERFACES_I_PLAYER_CODE_H

#include "player_wrapper/player_wrapper_export.h"
#include "state/player_state.h"
#include <sstream>
#include <string>

namespace player_wrapper {

/**
 * Player AI interface
 */
class PLAYER_WRAPPER_EXPORT IPlayerCode {
  protected:
	/**
	 * Player's debug logs
	 *
	 * To be returned and cleared every turn
	 */
	std::ostringstream logr;

  public:
	/**
	 * Player AI update function (main logic of the AI)
	 *
	 * Takes in a player state, allows player to read and write to it, and
	 * returns the modified player state
	 *
	 * @param[in]  state  The player state
	 *
	 * @return     The new player state
	 */
	virtual player_state::State Update(player_state::State state) = 0;

	/**
	 * Gets and clears player's debug logs
	 *
	 * @return     Debug logs
	 */
	virtual std::string GetAndClearDebugLogs() {
		auto logs = this->logr.str();
		this->logr.str("");
		return logs;
	}

	/**
	 * Destructor
	 */
	virtual ~IPlayerCode() {}
};
}

#endif
