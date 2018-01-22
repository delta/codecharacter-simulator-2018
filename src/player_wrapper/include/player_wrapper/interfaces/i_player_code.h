/**
 * @file i_player_code.h
 * Interface for the player AI
 */

#ifndef PLAYER_WRAPPER_INTERFACES_I_PLAYER_CODE_H
#define PLAYER_WRAPPER_INTERFACES_I_PLAYER_CODE_H

#include "player_wrapper/player_wrapper_export.h"
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
	 */
	virtual void Update() = 0;

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
