/**
 * @file shared_memory_player.cpp
 * Definitions for shared memory client
 */

#include "drivers/shared_memory_utils/shared_memory_player.h"
#include <cstring>

namespace drivers {

using namespace boost::interprocess;

SharedMemoryPlayer::SharedMemoryPlayer(std::string shared_memory_name)
    : shared_memory(open_only, shared_memory_name.c_str()) {}

SharedBuffer *SharedMemoryPlayer::GetBuffer() {
	return this->shared_memory.find<SharedBuffer>(unique_instance).first;
}
}
