/**
 * @file main_driver.cpp
 * Contains definitions for the main driver
 */

#include "drivers/main_driver.h"
#include <fstream>

namespace drivers {

MainDriver::MainDriver(
    std::unique_ptr<state::IStateSyncer> state_syncer,
    std::vector<std::unique_ptr<SharedMemoryMain>> shared_memories,
    int64_t player_instruction_limit_turn,
    int64_t player_instruction_limit_game, int64_t max_no_turns,
    int64_t player_count, Timer::Interval game_duration,
    std::unique_ptr<logger::ILogger> logger, std::string log_file_name)
    : state_syncer(std::move(state_syncer)),
      shared_memories(std::move(shared_memories)),
      player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game),
      max_no_turns(max_no_turns), player_count(player_count),
      is_game_timed_out(false), game_timer(), game_duration(game_duration),
      logger(std::move(logger)), log_file_name(log_file_name) {
	for (auto &shared_memory : this->shared_memories) {
		// Get pointers to shared memory and store
		SharedBuffer *shared_buffer = shared_memory->GetBuffer();
		shared_buffers.push_back(shared_buffer);

		// Store pointers to player state
		this->player_states.push_back(&shared_buffer->player_state);
	}
}

const std::vector<PlayerResult> MainDriver::Start() {
	// Initialize contents of shared memory
	for (int cur_player_id = 0; cur_player_id < this->player_count;
	     ++cur_player_id) {
		this->shared_buffers[cur_player_id]->is_player_running = false;
		this->shared_buffers[cur_player_id]->instruction_counter = 0;
	}

	// Initialize player states with contents of main state
	this->state_syncer->UpdatePlayerStates(this->player_states);

	// Start a timer. Game is invalid if it does not complete within the timer
	// limit
	this->is_game_timed_out = false;
	this->game_timer.Start(this->game_duration,
	                       [this]() { this->is_game_timed_out = true; });

	// Run the game and return results
	return this->Run();
}

const std::vector<PlayerResult> MainDriver::Run() {
	// Initializing stuff...
	std::vector<bool> skip_player_turn(this->player_count, false);
	std::vector<PlayerResult> player_results(
	    this->player_count, PlayerResult{0, PlayerResult::Status::UNDEFINED});
	std::vector<int64_t> player_scores(this->player_count, 0);
	bool instruction_count_exceeded = false;

	std::ofstream log_file(log_file_name, std::ios::out | std::ios::binary);

	// Main loop that runs every turn
	for (int i = 0; i < this->max_no_turns; ++i) {
		// Loop over each player
		for (int cur_player_id = 0; cur_player_id < this->player_count;
		     ++cur_player_id) {
			// Let player do his updates
			this->shared_buffers[cur_player_id]->is_player_running = true;

			// Wait for updates (or the timer)
			while (this->shared_buffers[cur_player_id]->is_player_running &&
			       !this->is_game_timed_out)
				;

			// Check for instruction counter to see if player has exceeded some
			// limit
			if (this->shared_buffers[cur_player_id]->instruction_counter >
			    this->player_instruction_limit_game) {
				player_results[cur_player_id].status =
				    PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT;
				instruction_count_exceeded = true;
			} else if (this->shared_buffers[cur_player_id]
			               ->instruction_counter >
			           this->player_instruction_limit_turn) {
				skip_player_turn[cur_player_id] = true;
			} else {
				skip_player_turn[cur_player_id] = false;
			}

			// Write the turn's instruction counts
			logger->LogInstructionCount(
			    static_cast<state::PlayerId>(cur_player_id),
			    this->shared_buffers[cur_player_id]->instruction_counter);
		}

		// If the game instruction count has been exceeded by some player, game
		// is forfeit
		// If the game timer has expired, the game has to stop
		if (instruction_count_exceeded || this->is_game_timed_out) {
			logger->LogFinalGameParams();
			logger->WriteGame(log_file);
			this->game_timer.Cancel();
			return player_results;
		}

		// If we're here, the game is not yet over

		// Validate and run the player's commands. Skips a player if they have
		// exceeded turn instruction limit
		this->state_syncer->ExecutePlayerCommands(this->player_states,
		                                          skip_player_turn);
		// Update the main state
		this->state_syncer->UpdateMainState();
		// Write the updated main state back to the player's state copies
		this->state_syncer->UpdatePlayerStates(this->player_states);
	}

	// Done with the game now

	// Get the players' scores
	player_scores = this->state_syncer->GetScores();

	// Write the player results
	for (int i = 0; i < this->player_count; ++i) {
		player_results[i] =
		    PlayerResult{player_scores[i], PlayerResult::Status::NORMAL};
	}

	logger->LogFinalGameParams();
	logger->WriteGame(log_file);
	this->game_timer.Cancel();
	return player_results;
}
}
