/**
 * @file i_logger.h
 * Declarations for the logger interface
 */

#ifndef LOGGER_I_LOGGER_H
#define LOGGER_I_LOGGER_H

#include "logger/error_type.h"
#include "logger/logger_export.h"
#include "state/interfaces/i_state.h"
#include <ostream>
#include <string>

namespace logger {

/**
 * Interface to log information from state
 */
class LOGGER_EXPORT ILogger {
  public:
	virtual ~ILogger(){};

	/**
	 * Takes a pointer to the main state, and logs all information
	 *
	 * @param[in]   state   Pointer to state
	 */
	virtual void LogState(state::IState *state) = 0;

	/**
	 * Takes a player and corresponding instruction count, and logs it in the
	 * current turn's game frame
	 *
	 * @param[in]   player_id   Player identifier
	 * @param[in]   count       Instruction count
	 */
	virtual void LogInstructionCount(state::PlayerId player_id,
	                                 int64_t count) = 0;

	/**
	 * Takes a player and the error, and logs it into the state. Every distinct
	 * string is assigned an error code and stored in the error_map
	 *
	 * @param[in]   player_id    The player identifier
	 * @param[in]   error_type   The error type
	 * @param[in]   message      The error string
	 */
	virtual void LogError(state::PlayerId player_id, ErrorType error_type,
	                      std::string message) = 0;

	/**
	 * Logs final game parameters, should be called once, right before logging
	 * state to stream (i.e before calling WriteGame)
	 */
	virtual void LogFinalGameParams() = 0;

	/**
	 * Writes the complete serialized logs to stream
	 */
	virtual void WriteGame(std::ostream &write_stream) = 0;
};
}

#endif
