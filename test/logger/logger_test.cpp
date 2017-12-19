#include "game.pb.h"
#include "logger/logger.h"
#include "state/mocks/state_mock.h"
#include "gtest/gtest.h"
#include <sstream>

using namespace std;
using namespace ::testing;
using namespace state;
using namespace logger;

class LoggerTest : public testing::Test {
  protected:
	std::unique_ptr<Logger> logger;
	std::unique_ptr<StateMock> state;

	LoggerTest()
	    : logger(make_unique<Logger>()), state(make_unique<StateMock>()) {}
};

TEST_F(LoggerTest, SimpleMoneyTest) {
	vector<int64_t> money1 = {400, 500};
	vector<int64_t> money2 = {300, 600};
	EXPECT_CALL(*state, GetMoney())
	    .Times(2)
	    .WillOnce(Return(money1))
	    .WillOnce(Return(money2));

	logger->LogState(state.get());
	logger->LogState(state.get());

	ostringstream str_stream;
	logger->WriteGame(str_stream);
	string result_string = str_stream.str();

	auto game = make_unique<proto::Game>();
	game->ParseFromString(result_string);

	ASSERT_EQ(game->states(0).money(0), money1[0]);
	ASSERT_EQ(game->states(0).money(1), money1[1]);
	ASSERT_EQ(game->states(1).money(0), money2[0]);
	ASSERT_EQ(game->states(1).money(1), money2[1]);
}
