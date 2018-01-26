#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include "logger/mocks/logger_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "gtest/gtest.h"
#include <atomic>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>

using namespace ::testing;
using namespace std;
using namespace state;
using namespace drivers;
using namespace logger;

class MainDriverTest : public testing::Test {
  protected:
	unique_ptr<MainDriver> driver;

	const static vector<string> shared_memory_names;

	const static int player_count;

	const static int num_turns;

	const static int time_limit_ms;

	const static int turn_instruction_limit;

	const static int game_instruction_limit;

	// Returns a new mock main driver
	static unique_ptr<MainDriver>
	CreateMockMainDriver(unique_ptr<StateSyncerMock> state_syncer_mock,
	                     unique_ptr<LoggerMock> v_logger) {
		vector<unique_ptr<SharedMemoryMain>> shm;
		for (const auto &shm_name : shared_memory_names) {
			// Remove shm if it already exists
			boost::interprocess::shared_memory_object::remove(shm_name.c_str());
			// Create new shm
			shm.emplace_back(new SharedMemoryMain(shm_name, false, 0,
			                                      player_state::State()));
		}

		return unique_ptr<MainDriver>(new MainDriver(
		    move(state_syncer_mock), move(shm), turn_instruction_limit,
		    game_instruction_limit, num_turns, player_count,
		    Timer::Interval(time_limit_ms), move(v_logger), "game.log"));
	}

  public:
	MainDriverTest() : driver(nullptr) {}
};

const vector<string> MainDriverTest::shared_memory_names = {"ShmTest1",
                                                            "ShmTest2"};
const int MainDriverTest::player_count = 2;
const int MainDriverTest::num_turns = pow(10, 4);
const int MainDriverTest::time_limit_ms = 1000;
const int MainDriverTest::turn_instruction_limit = 5;
const int MainDriverTest::game_instruction_limit = 10;

TEST_F(MainDriverTest, CleanRun) {
	// Expect a lock-free atomic_bool as only they will definitely work in shm.
	// TODO: find some way to use atomic_flag, or something else that's fast and
	// lock-free.
	EXPECT_EQ(ATOMIC_BOOL_LOCK_FREE, 2);

	// Declaring mock state syncer and setting expectations
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// Skip first half of turns...
	EXPECT_CALL(*state_syncer_mock,
	            ExecutePlayerCommands(_, vector<bool>(player_count, false)))
	    .Times(num_turns / 2);

	// ... and run the second half of turns
	EXPECT_CALL(*state_syncer_mock,
	            ExecutePlayerCommands(_, vector<bool>(player_count, true)))
	    .Times(num_turns / 2);

	EXPECT_CALL(*state_syncer_mock, UpdateMainState()).Times(num_turns);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_)).Times(num_turns + 1);
	EXPECT_CALL(*state_syncer_mock, GetScores())
	    .WillOnce(Return(vector<int64_t>(player_count, 10)));

	// Declare mock logger and setting expectations
	unique_ptr<LoggerMock> v_logger(new LoggerMock());

	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns);
	EXPECT_CALL(*v_logger, LogFinalGameParams()).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	// Start main driver on new thread
	vector<PlayerResult> player_results;
	thread main_runner(
	    [this, &player_results] { player_results = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < player_count; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	// Wait for player processes to wrap up
	for (auto &runner : player_runners) {
		runner.join();
	}
	// Wait for main driver to wrap up
	main_runner.join();

	// Number of result structs should equal number of players
	EXPECT_EQ(player_results.size(), player_count);

	// Everyone gets a score of 10 and status is normal as game finished
	// normally
	for (auto result : player_results) {
		EXPECT_EQ(result.score, 10);
		EXPECT_EQ(result.status, PlayerResult::Status::NORMAL);
	}
}

// Tests for case when player exits/crashes before game ends
// Main driver should simply time out and exit cleanly
TEST_F(MainDriverTest, EarlyPlayerExit) {
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// Expect only half the number of turns to be run
	EXPECT_CALL(*state_syncer_mock, ExecutePlayerCommands(_, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdateMainState()).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*state_syncer_mock, GetScores()).Times(0);

	// Logger called num_turns/2 + 1 times before the driver exits
	unique_ptr<LoggerMock> v_logger(new LoggerMock());
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogFinalGameParams()).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	vector<PlayerResult> player_results;
	thread main_runner(
	    [this, &player_results] { player_results = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < player_count; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns / 2 << ' '
		               << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	for (auto &runner : player_runners) {
		runner.join();
	}
	main_runner.join();

	// Number of result structs should equal number of players
	EXPECT_EQ(player_results.size(), player_count);

	// Results are undefined as game didn't complete
	for (auto result : player_results) {
		EXPECT_EQ(result.status, PlayerResult::Status::UNDEFINED);
	}
}

// Test for when a player exceeds game instruction limit
// Main driver should mark the player as having exceeded instruction limit
TEST_F(MainDriverTest, InstructionLimitReached) {
	unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

	// Expect only half the turns to run
	EXPECT_CALL(*state_syncer_mock, ExecutePlayerCommands(_, _))
	    .Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdateMainState()).Times(num_turns / 2);
	EXPECT_CALL(*state_syncer_mock, UpdatePlayerStates(_))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*state_syncer_mock, GetScores()).Times(0);

	unique_ptr<LoggerMock> v_logger(new LoggerMock());
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
	    .Times(num_turns / 2 + 1);
	EXPECT_CALL(*v_logger, LogFinalGameParams()).Times(1);
	EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

	driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

	vector<PlayerResult> player_results;
	thread main_runner(
	    [this, &player_results] { player_results = driver->Start(); });

	vector<thread> player_runners;
	for (int i = 0; i < player_count; ++i) {
		ostringstream command_stream;
		command_stream << "./main_driver_test_player " << shared_memory_names[i]
		               << ' ' << time_limit_ms << ' ' << num_turns / 2 - 1
		               << ' ' << turn_instruction_limit;
		string command = command_stream.str();
		player_runners.emplace_back(
		    [command] { EXPECT_EQ(system(command.c_str()), 0); });
	}

	for (auto &runner : player_runners) {
		runner.join();
	}

	// Simulating just being under instruction limit on n/2 turn by all players
	for (const auto &shm_name : shared_memory_names) {
		SharedMemoryPlayer shm_player(shm_name);
		SharedBuffer *buf = shm_player.GetBuffer();
		buf->instruction_counter = game_instruction_limit;
		buf->is_player_running = false;
	}

	// Simulating instruction limit exceeding on n/2 + 1 turn by all players
	for (const auto &shm_name : shared_memory_names) {
		SharedMemoryPlayer shm_player(shm_name);
		SharedBuffer *buf = shm_player.GetBuffer();
		buf->instruction_counter = game_instruction_limit + 1;
		buf->is_player_running = false;
	}

	main_runner.join();

	// Number of result structs should equal number of players
	EXPECT_EQ(player_results.size(), player_count);

	for (auto result : player_results) {
		EXPECT_EQ(result.status,
		          PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT);
	}
}
