/**
 * @file shared_memory_main.cpp
 * Definitions for owner of shared memory
 */

#include <cstring>
#include "drivers/shared_memory_utils/shared_memory_main.h"

namespace drivers {

SharedMemoryMain::SharedMemoryMain(std::string shared_memory_name)
: shared_memory_name(shared_memory_name) {
	using namespace boost::interprocess;

	//Create a shared memory object, throws if already created
	shared_memory_object shm(create_only, this->shared_memory_name.c_str(), read_write);

	//Set size of shared memory
	shm.truncate(sizeof(SharedBuffer));

	//Map whole shared memory in this process
	this->shared_memory_region = mapped_region(shm, read_write);
}

void SharedMemoryMain::Write(const SharedBuffer& shared_buffer) {
	std::memcpy(
		this->shared_memory_region.get_address(),
		&shared_buffer,
		sizeof(SharedBuffer)
	);
}

void SharedMemoryMain::Read(SharedBuffer& shared_buffer) {
	std::memcpy(
		&shared_buffer,
		this->shared_memory_region.get_address(),
		sizeof(SharedBuffer)
	);
}

SharedMemoryMain::~SharedMemoryMain() {
	boost::interprocess::shared_memory_object::remove(
		shared_memory_name.c_str());
}

}
