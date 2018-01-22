/**
 * @file logger.h
 * Declarations for the logger class
 */

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include "game.pb.h"
#include "logger/error_type.h"
#include "logger/interfaces/i_logger.h"
#include "logger/logger_export.h"
#include "state/interfaces/i_state.h"
#include <cstdint>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace logger {

/**
 * Entry for one tower in the logger
 */
struct TowerLogEntry {
	int64_t id;
	int64_t hp;
	int64_t level;
};

/**
 * Logger class that takes the current game state and logs information
 * Writes log to file system after game is complete
 */
class LOGGER_EXPORT Logger : public ILogger {
  private:
	/**
	 * Number of turns since the start of the game
	 */
	int64_t turn_count;

	/**
	 * A log of each player's towers, indexed by PlayerId
	 * Maintains the state of the game towers at the previous turn, so that
	 * only a diff of the changes in towers needs to be logged
	 */
	std::vector<std::vector<TowerLogEntry>> tower_logs;

	/**
	 * Protobuf object holding complete game logs
	 */
	std::unique_ptr<proto::Game> logs;

	/**
	 * Stores the instruction counts until they are written into the log along
	 * with the remainig state data, every turn
	 */
	std::vector<int64_t> instruction_counts;

	/**
	 * Map holding mapping of error strings to error codes
	 */
	std::unordered_map<std::string, int64_t> error_map;

	/**
	 * Holds an incrementing value to assign each error a unique code
	 */
	int64_t current_error_code;

	/**
	 * Holds the errors that occured in a particular move, indexed by player_id
	 */
	std::vector<std::vector<int64_t>> errors;

	/**
	 * Number of instructions exceeding which the turn is forfeit
	 */
	int64_t player_instruction_limit_turn;

	/**
	 * Number of instructions exceeding which the game is forfeit
	 */
	int64_t player_instruction_limit_game;

  public:
	/**
	 * Constructor for the Logger class
	 */
	Logger(int64_t player_instruction_limit_turn,
	       int64_t player_instruction_limit_game);

	/**
	 * @see ILogger#LogState
	 */
	void LogState(state::IState *state) override;

	/**
	 * @see ILogger#LogInstructionCount
	 */
	void LogInstructionCount(state::PlayerId player_id, int64_t count) override;

	/**
	 * @see ILogger#LogError
	 */
	void LogError(state::PlayerId player_id, ErrorType error_type,
	              std::string message) override;

	/**
	 * @see ILogger#LogFinalGameParams
	 */
	void LogFinalGameParams() override;

	/**
	 * @see ILogger#WriteGame
	 * Defaults to std::cout when no stream passed
	 */
	void WriteGame(std::ostream &write_stream = std::cout) override;
};
}

#endif
