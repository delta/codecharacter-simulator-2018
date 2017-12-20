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

TEST_F(LoggerTest, EmptyTest) {}
