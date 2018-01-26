/**
 * @file shared_buffer.h
 * Declaration for buffer object in shared memory
 */

#ifndef DRIVERS_SHARED_MEMORY_UTILS_SHARED_BUFFER_H
#define DRIVERS_SHARED_MEMORY_UTILS_SHARED_BUFFER_H

#include "drivers/drivers_export.h"
#include "state/player_state.h"
#include <atomic>

namespace drivers {

/**
 * Struct for using as buffer in shared memory
 */
struct DRIVERS_EXPORT SharedBuffer {
	SharedBuffer(bool is_player_running, int64_t instruction_counter,
	             const player_state::State &player_state);

	/**
	 * True if the player process is executing its turn, false otherwise
	 */
	std::atomic_bool is_player_running;

	/**
	 * Count of the number of instructions executed in the present turn
	 */
	std::atomic<int64_t> instruction_counter;

	/**
	 * Player's copy of the state with limited information
	 */
	player_state::State player_state;
};
}

#endif
