/**
 * @file player_driver.h
 * Declaration for player driver class that drives the game
 */

#ifndef DRIVERS_PLAYER_DRIVER_H
#define DRIVERS_PLAYER_DRIVER_H

#include "drivers/drivers_export.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "player_wrapper/player_code_wrapper.h"
#include "state/player_state.h"
#include <memory>

namespace drivers {

/**
 * Drives the game by syncing states
 */
class DRIVERS_EXPORT PlayerDriver {
  private:
	/**
	 * Number of LLVM IR instructions executed by the player
	 */
	static uint64_t instruction_count;

	/**
	 * An instance of the player code wrapper
	 */
	std::unique_ptr<player::PlayerCodeWrapper> player_code_wrapper;

	/**
	 * Shared memory of the player to write count to
	 */
	std::unique_ptr<SharedMemoryPlayer> shm_player;

	/**
	 * Writes the count to shared memory
	 */
	void WriteCountToShm();

  public:
	/**
	 * Constructor
	 *
	 * @param      player_code_wrapper  The player code wrapper
	 * @param      shm_player           The player's shared memory to write the
	 * count to
	 */
	PlayerDriver(std::unique_ptr<player::PlayerCodeWrapper> player_code_wrapper,
	             std::unique_ptr<SharedMemoryPlayer> shm_player);

	/**
	 * Increment instruction_count by count
	 *
	 * @param  count  The count
	 */
	static void IncrementCount(uint64_t count);

	/**
	 * Gets the instruction_count
	 *
	 * @return     The count.
	 */
	uint64_t GetCount();

	/**
	 * Runs the player's update and then writes the count to shared memory
	 */
	void Update();
};
}

#endif
