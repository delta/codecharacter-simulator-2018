#include "constants/constants.h"
#include "drivers/player_driver.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include "player_code/player_code.h"
#include "player_wrapper/player_code_wrapper.h"
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace drivers;
using namespace player_wrapper;
using namespace player_code;

const std::string player_debug_log_ext = ".dlog";
const std::string debug_logs_turn_prefix =
    ">>>>>>>>>>>>>>>>>>>START OF TURN LOG<<<<<<<<<<<<<<<<<<<<\n";
const std::string debug_logs_truncate_message =
    "(logs truncated due to excessive size)\n";
const int64_t max_debug_logs_turn_length = 10000;

std::unique_ptr<PlayerDriver>
BuildPlayerDriver(std::string shm_name, std::string player_debug_log_file) {
	auto shm_player = std::make_unique<SharedMemoryPlayer>(shm_name);

	auto player_code_wrapper = std::make_unique<PlayerCodeWrapper>(
	    std::make_unique<PlayerCode>(&shm_player->GetBuffer()->player_state));

	return std::make_unique<PlayerDriver>(
	    std::move(player_code_wrapper), std::move(shm_player), NUM_TURNS,
	    Timer::Interval(GAME_DURATION_MS), player_debug_log_file,
	    debug_logs_turn_prefix, debug_logs_truncate_message,
	    max_debug_logs_turn_length);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << argv[0] << " Requires SHM player name as arg";
		exit(EXIT_FAILURE);
	}
	std::string shm_name(argv[1]);

	std::cout << "Running " << argv[0] << " ..." << std::endl;
	auto driver = std::move(BuildPlayerDriver(
	    shm_name, std::string(argv[0]) + player_debug_log_ext));

	driver->Start();
	std::cout << argv[0] << " Done!" << std::endl;
	return 0;
}
