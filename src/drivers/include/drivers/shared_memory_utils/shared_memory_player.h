/**
 * @file shared_memory_player.h
 * Declaration for shared memory client
 */

#ifndef DRIVERS_SHARED_MEMORY_UTILS_SHARED_MEMORY_PLAYER_H
#define DRIVERS_SHARED_MEMORY_UTILS_SHARED_MEMORY_PLAYER_H

#define BOOST_DATE_TIME_NO_LIB

#include "boost/interprocess/managed_shared_memory.hpp"
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
	boost::interprocess::managed_shared_memory shared_memory;

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
	 * Gets pointer to shared memory
	 *
	 * @return     The pointer
	 */
	SharedBuffer *GetBuffer();
};
}

#endif
