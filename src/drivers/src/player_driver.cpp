/**
 * @file player_driver.cpp
 * Contains definitions for the player driver
 */

#include "drivers/player_driver.h"
#include <fstream>

namespace drivers {

std::atomic<uint64_t> PlayerDriver::instruction_count(0);

PlayerDriver::PlayerDriver(
    std::unique_ptr<player_wrapper::PlayerCodeWrapper> player_code_wrapper,
    std::unique_ptr<drivers::SharedMemoryPlayer> shm_player,
    int64_t max_no_turns, Timer::Interval game_duration,
    std::string player_debug_log_file, std::string debug_logs_turn_prefix,
    std::string debug_logs_truncate_message, int64_t max_debug_logs_turn_length)
    : player_code_wrapper(std::move(player_code_wrapper)),
      shm_player(std::move(shm_player)),
      shared_buffer(this->shm_player->GetBuffer()), max_no_turns(max_no_turns),
      is_game_timed_out(false), game_timer(), game_duration(game_duration),
      player_debug_log_file(player_debug_log_file),
      debug_logs_turn_prefix(debug_logs_turn_prefix),
      debug_logs_truncate_message(debug_logs_truncate_message),
      max_debug_logs_turn_length(max_debug_logs_turn_length) {}

void PlayerDriver::IncrementCount(uint64_t count) {
	instruction_count += count;
}

uint64_t PlayerDriver::GetCount() { return instruction_count; }

void PlayerDriver::WriteCountToShm() {
	this->shared_buffer->instruction_counter = instruction_count;
}

void PlayerDriver::Start() {
	// Start a timer. Game is invalid if it does not complete within the timer
	// limit
	this->is_game_timed_out = false;
	this->game_timer.Start(this->game_duration,
	                       [this]() { this->is_game_timed_out = true; });

	// Run the game and return results
	return this->Run();
}

void PlayerDriver::Run() {
	// Loop to run the player's code every turn
	for (int i = 0; i < this->max_no_turns; ++i) {

		// Wait for the main driver to synchronize states or until the game has
		// timed out
		while (!this->shared_buffer->is_player_running &&
		       !this->is_game_timed_out)
			;

		// If overall game time limit was exceeded, stop the player's code
		if (this->is_game_timed_out)
			break;

		// Run player's code and get number of instructions they used and their
		// debug logs
		instruction_count = 0;
		auto logs = this->player_code_wrapper->Update(
		    this->shared_buffer->player_state);
		this->player_debug_logs << this->debug_logs_turn_prefix
		                        << logs.substr(0, max_debug_logs_turn_length);

		// Truncate debug logs if they're too long and add a truncation message
		if (logs.length() > this->max_debug_logs_turn_length) {
			this->player_debug_logs << this->debug_logs_truncate_message;
		}

		this->WriteCountToShm();

		// Let the main driver synchronize states now
		this->shared_buffer->is_player_running = false;
	}

	// Open debug log file and store player's debug logs in it
	std::ofstream debug_log_file(this->player_debug_log_file);
	debug_log_file << this->player_debug_logs.str();
	this->game_timer.Cancel();
}
}
