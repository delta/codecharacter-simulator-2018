/**
 * @file i_logger.h
 * Declarations for the logger interface
 */

#ifndef LOGGER_I_LOGGER_H
#define LOGGER_I_LOGGER_H

#include "logger/logger_export.h"
#include "state/interfaces/i_state.h"
#include <ostream>

namespace logger {

/**
 * Interface to log information from state
 */
class LOGGER_EXPORT ILogger {
  public:
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
	 * Writes the complete serialized logs to stream
	 */
	virtual void WriteGame(std::ostream &write_stream) = 0;
};
}

#endif
