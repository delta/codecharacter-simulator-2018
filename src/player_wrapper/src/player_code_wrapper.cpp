/**
 * @file player_code_wrapper.cpp
 * Contains definitions for the player code wrapper
 */

#include "player_wrapper/player_code_wrapper.h"

namespace player_wrapper {

PlayerCodeWrapper::PlayerCodeWrapper(std::unique_ptr<IPlayerCode> player_code)
    : player_code(std::move(player_code)) {}

void PlayerCodeWrapper::Update() { player_code->Update(); }
}
