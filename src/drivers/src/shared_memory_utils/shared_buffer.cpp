/**
 * @file shared_buffer.cpp
 * Definitions for buffer for shared memory
 */

#include "drivers/shared_memory_utils/shared_buffer.h"

namespace drivers {

SharedBuffer::SharedBuffer(bool is_player_running, int64_t instruction_counter,
                           const player_state::State &player_state)
    : is_player_running(is_player_running),
      instruction_counter(instruction_counter), player_state(player_state) {}
}
