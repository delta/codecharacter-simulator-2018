/**
 * @file shared_memory_main.cpp
 * Definitions for owner of shared memory
 */

#include <cstring>
#include "drivers/shared_memory_utils/shared_memory_main.h"

namespace drivers {

using namespace boost::interprocess;

SharedMemoryMain::SharedMemoryMain(
	std::string shared_memory_name,
	bool is_player_running,
	int64_t instruction_counter,
	const state::PlayerState& player_state
	) :
	shared_memory_name(shared_memory_name),
	// Creating shared memory
	shared_memory(create_only, shared_memory_name.c_str(), 65536)
{
	// Constructing unique instance of SharedBuffer in shared memory
	this->shared_memory.construct<SharedBuffer>(unique_instance)(
		is_player_running,
		instruction_counter,
		player_state
	);
}

SharedBuffer * SharedMemoryMain::GetBuffer() {
	return this->shared_memory.find<SharedBuffer>(unique_instance).first;
}


SharedMemoryMain::~SharedMemoryMain() {
	this->shared_memory.destroy<SharedBuffer>(unique_instance);
	shared_memory_object::remove(shared_memory_name.c_str());
}

}
