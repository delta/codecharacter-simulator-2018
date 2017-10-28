#include "drivers/player_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"

#include "player_code/test/player_code_test_0.h"
#include "player_wrapper/player_code_wrapper.h"

#include "state/player_state.h"

#include "gtest/gtest.h"

using namespace drivers;
using namespace player;
using namespace state;
using namespace std;

const string shm_name = "LLVMInstTest";

void CheckPass(unique_ptr<IPlayerCode> player_code,
    unique_ptr<SharedMemoryPlayer> shm_player) {

	SharedBuffer* buf = shm_player->GetBuffer();
	buf->instruction_counter = 0;

	unique_ptr<PlayerCodeWrapper> player_code_wrapper(
	    new PlayerCodeWrapper(move(player_code)));
	PlayerDriver* driver
	    = new PlayerDriver(move(player_code_wrapper), move(shm_player));

	// Skip the first loop: The pass increments count for initialization
	// of variables and header includes which is a one time thing
	int count_init = driver->GetCount();
	driver->Update();
	int count_one = driver->GetCount() - count_init;
	driver->Update();
	int count_two = driver->GetCount() - count_init;

	ASSERT_GT(buf->instruction_counter, 0);

	ASSERT_EQ(count_two, count_one * 2);

	ASSERT_EQ(buf->instruction_counter, count_two + count_init);
}

TEST(LLVMPassTest, InstructionCountTest) {
	boost::interprocess::shared_memory_object::remove(shm_name.c_str());

	PlayerState player_state = PlayerState();
	SharedMemoryMain shm_main(shm_name, false, 0, player_state);
	unique_ptr<SharedMemoryPlayer> shm_player(new SharedMemoryPlayer(shm_name));

	unique_ptr<PlayerCode0> player_code(new PlayerCode0(&player_state));

	CheckPass(move(player_code), move(shm_player));
}
