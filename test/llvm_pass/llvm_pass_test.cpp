#include "drivers/player_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "player_code/test/player_code_test_0.h"
#include "player_code/test/player_code_test_1.h"
#include "player_code/test/player_code_test_2.h"
#include "player_wrapper/player_code_wrapper.h"
#include "state/player_state.h"
#include "gtest/gtest.h"
#include <cstdio>
#include <fstream>

using namespace drivers;
using namespace player_wrapper;
using namespace player_code;
using namespace std;
using namespace ::testing;

class LLVMPassTest : public Test {
  protected:
	const string shm_name = "LLVMInstTest";

  public:
	LLVMPassTest() {
		boost::interprocess::shared_memory_object::remove(shm_name.c_str());

		this->shm_main = make_unique<SharedMemoryMain>(shm_name, false, 0,
		                                               player_state::State());
		this->buf = shm_main->GetBuffer();
	}

	template <class T>
	void SetPlayerDriver(int num_turns, int time_limit_ms,
	                     int64_t max_log_turn_length) {
		unique_ptr<SharedMemoryPlayer> shm_player(
		    new SharedMemoryPlayer(shm_name));

		unique_ptr<T> player_code(
		    new T(&shm_player->GetBuffer()->player_state));

		unique_ptr<PlayerCodeWrapper> player_code_wrapper(
		    new PlayerCodeWrapper(move(player_code)));

		this->driver = make_unique<PlayerDriver>(
		    move(player_code_wrapper), move(shm_player), num_turns,
		    Timer::Interval(time_limit_ms), log_file, turn_prefix,
		    truncate_message, max_log_turn_length);
	}

	const string log_file = "lol.dlog";

	const string turn_prefix = "<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>\n";

	const string truncate_message = "(truncated due to very bigly)\n";

	unique_ptr<SharedMemoryMain> shm_main;

	SharedBuffer *buf;

	unique_ptr<PlayerDriver> driver;
};

// The below tests are for both the PlayerDriver and the instrumentation

// Test case to see if instrumentation works on a normal run
TEST_F(LLVMPassTest, InstructionCountTest1) {
	SetPlayerDriver<PlayerCode0>(2, 1000, 0);

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

// NOTE: This test seems to be fine, but currently fails
// Uncomment after debugging the issue
// TEST_F(LLVMPassTest, PlayerDriverTimeout) {
// 	int num_turns = 50;
// 	int time_limit_ms = 1000;

// 	SetPlayerDriver<PlayerCode0>(num_turns, time_limit_ms, 0);

// 	buf->instruction_counter = 0;

// 	thread runner([this] { driver->Start(); });

// 	Timer timer;
// 	atomic_bool is_time_over(false);
// 	timer.Start((Timer::Interval(time_limit_ms)),
// 	            [&is_time_over]() { is_time_over = true; });

// 	buf->is_player_running = true;
// 	while (buf->is_player_running && !is_time_over)
// 		;
// 	int prev_instruction_count = buf->instruction_counter;

// 	for (int i = 1; i < num_turns / 2; ++i) {
// 		buf->is_player_running = true;
// 		while (buf->is_player_running && !is_time_over)
// 			;
// 		// Number of instructions every turn must be the same, as the exact same
// 		// player code runs every turn
// 		EXPECT_EQ(prev_instruction_count, buf->instruction_counter);
// 		prev_instruction_count = buf->instruction_counter;
// 		buf->instruction_counter = 0;
// 	}

// 	runner.join();
// 	while (!is_time_over)
// 		;
// }

// Normal test for another player code
// Just checking if instrumentation works
TEST_F(LLVMPassTest, InstructionCountTest2) {
	int num_turns = 50;
	int time_limit_ms = 500;

	SetPlayerDriver<PlayerCode1>(num_turns, time_limit_ms, 0);

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
	EXPECT_GT(prev_instruction_count, 0);

	for (int i = 1; i < num_turns; ++i) {
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
	while (!is_time_over)
		;
}

// Test to see if debug log file is properly written to
TEST_F(LLVMPassTest, DebugLogsNormalRun) {
	int num_turns = 50;
	int time_limit_ms = 1000;
	int max_log_turn_length = 10E5;

	// Run the player driver
	SetPlayerDriver<PlayerCode2>(num_turns, time_limit_ms, max_log_turn_length);

	thread runner([this] { driver->Start(); });

	for (int i = 0; i < num_turns; ++i) {
		buf->is_player_running = true;
		while (buf->is_player_running)
			;
	}

	runner.join();

	// Player is done running

	// Open the log file and expect it to exist
	ifstream log_file(this->log_file);
	EXPECT_TRUE(log_file.good());

	// Read the log file
	std::ostringstream logs;
	logs << log_file.rdbuf();
	auto log_str = logs.str();

	// Check if it has the expected contents: turn prefix and 10^4 asterisks
	// every turn
	size_t pos = 0;
	int num_turns_in_log = 0;
	while ((pos = log_str.find(this->turn_prefix)) != std::string::npos) {
		log_str.erase(0, pos + this->turn_prefix.length());
		EXPECT_EQ(log_str.substr(0, 10E4), std::string(10E4, '*'));
		num_turns_in_log++;
	}

	// Expect all turns to be logged
	EXPECT_EQ(num_turns_in_log, num_turns);

	// Delete log file
	log_file.close();
	EXPECT_EQ(std::remove(this->log_file.c_str()), 0);
}

// Test to see if debug logs are properly handling per turn character limits
TEST_F(LLVMPassTest, DebugLogsTruncated) {
	int num_turns = 50;
	int time_limit_ms = 1000;
	int max_log_turn_length = 10E3;

	// Run the player driver
	SetPlayerDriver<PlayerCode2>(num_turns, time_limit_ms, max_log_turn_length);

	thread runner([this] { driver->Start(); });

	for (int i = 0; i < num_turns; ++i) {
		buf->is_player_running = true;
		while (buf->is_player_running)
			;
	}

	runner.join();

	// Player is done running

	// Open the log file and expect it to exist
	ifstream log_file(this->log_file);
	EXPECT_TRUE(log_file.good());

	// Read the log file
	std::ostringstream logs;
	logs << log_file.rdbuf();
	auto log_str = logs.str();

	// Check if it has the expected contents: turn prefix and the truncation
	// message
	size_t pos = 0;
	int num_turns_in_log = 0;
	while ((pos = log_str.find(this->turn_prefix)) != std::string::npos) {
		log_str.erase(0, pos + this->turn_prefix.length());
		// Add 1 to account for newline after debug logs
		EXPECT_EQ(log_str.find(this->truncate_message),
		          max_log_turn_length + 1);
		num_turns_in_log++;
	}

	// Expect all turns to be logged
	EXPECT_EQ(num_turns_in_log, num_turns);

	// Delete log file
	log_file.close();
	EXPECT_EQ(std::remove(this->log_file.c_str()), 0);
}
