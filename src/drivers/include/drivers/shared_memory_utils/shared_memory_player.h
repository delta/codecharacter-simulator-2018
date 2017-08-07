/**
 * @file shared_memory_player.h
 * Declaration for shared memory client
 */

#ifndef DRIVERS_SHARED_MEMORY_UTILS_SHARED_MEMORY_PLAYER_H
#define DRIVERS_SHARED_MEMORY_UTILS_SHARED_MEMORY_PLAYER_H

#define BOOST_DATE_TIME_NO_LIB

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "drivers/drivers_export.h"
#include "drivers/shared_memory_utils/shared_buffer.h"

namespace drivers {

/**
 * Wrapper for shared memory class that will be used by players (a client)
 */
class DRIVERS_EXPORT SharedMemoryPlayer {
private:

	/**
	 * Mapped region to write to and read from
	 */
	boost::interprocess::mapped_region shared_memory_region;

public:

	/**
	 * Opens existing shm with given name
	 *
	 * @param[in]  shared_memory_name  The shared memory name
	 *
	 * @throw      std::exception      If shm doesn't already exist
	 */
	SharedMemoryPlayer(std::string shared_memory_name);

	/**
	 * Writes buffer into shared memory
	 *
	 * @param[in]  shared_buffer  The buffer
	 */
	void Write(const SharedBuffer& shared_buffer);

	/**
	 * Reads from shared memory into buffer
	 *
	 * @param      shared_buffer  The buffer
	 */
	void Read(SharedBuffer& shared_buffer);
};

}

#endif
