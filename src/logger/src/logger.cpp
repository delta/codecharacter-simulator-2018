/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"

namespace logger {

Logger::Logger() : turn_count(0), logs(std::make_unique<proto::Game>()) {}

void Logger::LogState(state::IState *state) {
	// TODO: Define function

	// Dummy definition for testing
	proto::GameState *game_state = logs->add_states();

	std::vector<int64_t> money = state->GetMoney();
	for (auto player_money : money) {
		game_state->add_money(player_money);
	}
}

void Logger::WriteGame(std::ostream &write_stream) {
	logs->SerializeToOstream(&write_stream);
}
}
