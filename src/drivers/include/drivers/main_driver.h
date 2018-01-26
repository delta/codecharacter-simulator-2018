/**
 * @file main_driver.h
 * Declaration for main driver class that drives the game
 */

#ifndef DRIVERS_MAIN_DRIVER_H
#define DRIVERS_MAIN_DRIVER_H

#include "drivers/drivers_export.h"
#include "drivers/player_result.h"
#include "drivers/shared_memory_utils/shared_buffer.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/timer.h"
#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_state_syncer.h"
#include <atomic>
#include <string>
#include <thread>

namespace drivers {

/**
 * Drives the game by syncing states
 */
class DRIVERS_EXPORT MainDriver {
  private:
	/**
	 * Syncs the player's copy of the state with the main one
	 */
	std::unique_ptr<state::IStateSyncer> state_syncer;

	/**
	 * Shared memories via which player and main drivers communicate
	 */
	std::vector<std::unique_ptr<SharedMemoryMain>> shared_memories;

	/**
	 * Access pointer to the shared memory
	 */
	std::vector<SharedBuffer *> shared_buffers;

	/**
	 * Pointers to player state copies
	 */
	std::vector<player_state::State *> player_states;

	/**
	 * Instruction count limit.
	 *
	 * If player exceeds turn one, the player forfeits the turn.
	 * Exceeding the game one results in forfeiting the match.
	 */
	int64_t player_instruction_limit_turn, player_instruction_limit_game;

	/**
	 * Number of turns in the game
	 */
	int64_t max_no_turns;

	/**
	 * Number of players in the game
	 */
	int64_t player_count;

	/**
	 * true if the game has timed out without completing and Start has been
	 * called, false otherwise
	 */
	std::atomic_bool is_game_timed_out;

	/**
	 * Timer for the game
	 */
	Timer game_timer;

	/**
	 * Time limit for the game.
	 *
	 * Game must be completed within this time to be considered valid.
	 */
	Timer::Interval game_duration;

	/**
	 * Blocking function that runs the game
	 *
	 * @return     Results of the game indexed by player ID
	 */
	const std::vector<PlayerResult> Run();

	/**
	 * Instance of logger to write game to log file
	 */
	std::unique_ptr<logger::ILogger> logger;

	/**
	 * Filename to write the final game log to
	 */
	std::string log_file_name;

  public:
	/**
	 * Constructor
	 */
	MainDriver(std::unique_ptr<state::IStateSyncer> state_syncer,
	           std::vector<std::unique_ptr<SharedMemoryMain>> shared_memories,
	           int64_t player_instruction_limit_turn,
	           int64_t player_instruction_limit_game, int64_t max_no_turns,
	           int64_t player_count, Timer::Interval game_duration,
	           std::unique_ptr<logger::ILogger> logger,
	           std::string log_file_name);

	/**
	 * Blocking function that starts the game.
	 *
	 * Calls MainDriver::Run after setting up.
	 *
	 * @return     Results of the game indexed by player ID
	 */
	const std::vector<PlayerResult> Start();
};
}

#endif
