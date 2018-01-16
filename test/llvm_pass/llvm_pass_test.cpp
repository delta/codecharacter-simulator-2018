#include "drivers/player_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "player_code/test/player_code_test_0.h"
#include "player_wrapper/player_code_wrapper.h"
#include "state/player_state.h"
#include "gtest/gtest.h"

using namespace drivers;
using namespace player_wrapper;
using namespace player_code;
using namespace state;
using namespace std;
using namespace ::testing;

class LLVMPassTest : public Test {
  protected:
	const string shm_name = "LLVMInstTest";

  public:
	LLVMPassTest() {
		boost::interprocess::shared_memory_object::remove(shm_name.c_str());

		this->shm_main =
		    make_unique<SharedMemoryMain>(shm_name, false, 0, PlayerState());
		this->buf = shm_main->GetBuffer();
	}

	void SetPlayerDriver(int num_turns, int time_limit_ms) {
		unique_ptr<SharedMemoryPlayer> shm_player(
		    new SharedMemoryPlayer(shm_name));

		unique_ptr<PlayerCode0> player_code(
		    new PlayerCode0(&shm_player->GetBuffer()->player_state));

		unique_ptr<PlayerCodeWrapper> player_code_wrapper(
		    new PlayerCodeWrapper(move(player_code)));

		this->driver = make_unique<PlayerDriver>(
		    move(player_code_wrapper), move(shm_player), num_turns,
		    Timer::Interval(time_limit_ms));
	}

	unique_ptr<SharedMemoryMain> shm_main;

	SharedBuffer *buf;

	unique_ptr<PlayerDriver> driver;
};

// The below tests are for both the PlayerDriver and the instrumentation

// Test case to see if instrumentation works on a normal run
TEST_F(LLVMPassTest, InstructionCountTest) {
	SetPlayerDriver(2, 1000);

	buf->instruction_counter = 0;

	// Start the player driver
	thread runner([this] { driver->Start(); });

	buf->is_player_running = true;
	while (buf->is_player_running)
		;
	int count_one = driver->GetCount();
	buf->instruction_counter = 0;

	buf->is_player_running = true;
	while (buf->is_player_running)
		;
	int count_two = driver->GetCount();

	runner.join();

	ASSERT_GT(buf->instruction_counter, 0);

	ASSERT_EQ(count_two, count_one);

	ASSERT_EQ(buf->instruction_counter, count_two);
}

// Test case for game timing out before all turns complete
// Player driver needs to exit gracefully
// Instrumentation needs to work correctly
TEST_F(LLVMPassTest, PlayerDriverTimeout) {
	int num_turns = 50;
	int time_limit_ms = 1000;

	SetPlayerDriver(num_turns, time_limit_ms);

	buf->instruction_counter = 0;

	thread runner([this] { driver->Start(); });

	Timer timer;
	atomic_bool is_time_over(false);
	timer.Start((Timer::Interval(time_limit_ms)),
	            [&is_time_over]() { is_time_over = true; });

	buf->is_player_running = true;
	while (buf->is_player_running && !is_time_over)
		;
	int prev_instruction_count = buf->instruction_counter;

	for (int i = 1; i < num_turns / 2; ++i) {
		buf->is_player_running = true;
		while (buf->is_player_running && !is_time_over)
			;
		// Number of instructions every turn must be the same, as the exact same
		// player code runs every turn
		EXPECT_EQ(prev_instruction_count, buf->instruction_counter);
		prev_instruction_count = buf->instruction_counter;
		buf->instruction_counter = 0;
	}

	runner.join();
}
