/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"

namespace logger {

Logger::Logger() : turn_count(0), logs(std::make_unique<proto::Game>()) {}

void Logger::LogState(state::IState *state) {
	// TODO: Define function
}

void Logger::WriteGame(std::ostream &write_stream) {
	logs->SerializeToOstream(&write_stream);
}
}
