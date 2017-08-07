/**
 * @file shared_memory_player.cpp
 * Definitions for shared memory client
 */

#include <cstring>
#include "drivers/shared_memory_utils/shared_memory_player.h"

namespace drivers {

SharedMemoryPlayer::SharedMemoryPlayer(std::string shared_memory_name) {
	using namespace boost::interprocess;

	//Open a shared memory object, throws if does not exist
	shared_memory_object shm(open_only, shared_memory_name.c_str(), read_write);

	//Map whole shared memory in this process
	this->shared_memory_region = mapped_region(shm, read_write);
}

void SharedMemoryPlayer::Write(const SharedBuffer& shared_buffer) {
	std::memcpy(
		this->shared_memory_region.get_address(),
		&shared_buffer,
		sizeof(SharedBuffer)
	);
}

void SharedMemoryPlayer::Read(SharedBuffer& shared_buffer) {
	std::memcpy(
		&shared_buffer,
		this->shared_memory_region.get_address(),
		sizeof(SharedBuffer)
	);
}

}
