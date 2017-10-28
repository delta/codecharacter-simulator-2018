/**
 * @file player_driver.cpp
 * Contains definitions for the player driver
 */

#include "drivers/player_driver.h"

namespace drivers {

uint64_t PlayerDriver::instruction_count = 0;

PlayerDriver::PlayerDriver(
    std::unique_ptr<player::PlayerCodeWrapper> player_code_wrapper,
    std::unique_ptr<drivers::SharedMemoryPlayer> shm_player)
    : player_code_wrapper(std::move(player_code_wrapper)),
      shm_player(std::move(shm_player)) {}

void PlayerDriver::IncrementCount(uint64_t count) {
	instruction_count += count;
}

uint64_t PlayerDriver::GetCount() { return instruction_count; }

void PlayerDriver::WriteCountToShm() {
	shm_player->GetBuffer()->instruction_counter = instruction_count;
}

void PlayerDriver::Update() {
	player_code_wrapper->Update();
	WriteCountToShm();
}
}
