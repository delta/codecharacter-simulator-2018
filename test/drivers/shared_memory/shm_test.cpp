#include <cstdlib>
#include <stdexcept>
#include "drivers/shared_memory_utils/shared_buffer.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "state/player_state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace drivers;

const string shm_name = "ShmTest";

TEST(SharedMemoryUtilsTest, ValidWriteRead) {
	// Create owner of shm
	SharedMemoryMain shm_main(shm_name);

	// Write to shm
	SharedBuffer buf(true, 1, (state::PlayerState()));
	shm_main.Write(buf);

	// Spawn shm client to read from shm
	string command = "./shm_client " + shm_name;
	ASSERT_EQ(system(command.c_str()), 0) <<
		"If you are not running these tests from inside <install_location>/bin, cd there";
}

TEST(SharedMemoryUtilsTest, InvalidRead) {
	// Create client without owner
	EXPECT_THROW((SharedMemoryPlayer(shm_name)), std::exception);

	// Test if destructor for owner is working
	{
		SharedMemoryMain shm_main(shm_name);
	}

	EXPECT_THROW((SharedMemoryPlayer(shm_name)), std::exception);
}

TEST(SharedMemoryUtilsTest, InvalidCreate) {
	SharedMemoryMain shm_main(shm_name);
	// Cannot have two owners
	EXPECT_THROW((SharedMemoryMain(shm_name)), std::exception);
}
