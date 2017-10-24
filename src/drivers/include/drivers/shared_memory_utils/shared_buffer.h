/**
 * @file shared_buffer.h
 * Declaration for buffer object in shared memory
 */

#ifndef DRIVERS_SHARED_MEMORY_UTILS_SHARED_BUFFER_H
#define DRIVERS_SHARED_MEMORY_UTILS_SHARED_BUFFER_H

#include <atomic>
#include "drivers/drivers_export.h"
#include "drivers/spin_lock.h"
#include "state/player_state.h"

namespace drivers {

/**
 * Struct for using as buffer in shared memory
 */
struct DRIVERS_EXPORT SharedBuffer {
	SharedBuffer(
		bool is_player_running,
		int64_t instruction_counter,
		const state::PlayerState& player_state
	);

	/**
	 * True if the player process is executing its turn, false otherwise
	 */
	// std::atomic_bool is_player_running;
	
	SpinLock main_lock;
	SpinLock player_lock;
	/**
	 * Count of the number of instructions executed in the present turn
	 */
	std::atomic<int64_t> instruction_counter;

	/**
	 * Player's copy of the state with limited information
	 */
	state::PlayerState player_state;
};

}

#endif
