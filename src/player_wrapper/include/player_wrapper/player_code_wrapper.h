/**
 * @file player_code_wrapper.h
 * Declaration for class that wraps the players' code
 */

#ifndef PLAYER_PLAYER_CODE_WRAPPER_H
#define PLAYER_PLAYER_CODE_WRAPPER_H

#include "player_wrapper/interfaces/i_player_code.h"
#include <memory>

namespace player {

/**
 * Class that wraps the players' code
 */
class PLAYER_WRAPPER_EXPORT PlayerCodeWrapper {
  private:
	/**
	 * An instance of the player code
	 */
	std::unique_ptr<IPlayerCode> player_code;

  public:
	/**
	 * Constructor
	 *
	 * @param      player_code  The player code
	 */
	PlayerCodeWrapper(std::unique_ptr<IPlayerCode> player_code);

	/**
	 * Runs the player's update
	 */
	void Update();
};
}

#endif
