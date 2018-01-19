#ifndef TEST_LOGGER_MOCKS_LOGGER_H
#define TEST_LOGGER_MOCKS_LOGGER_H

#include "gmock/gmock.h"
#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_state.h"
#include "state/utilities.h"
#include <cstdint>
#include <ostream>
#include <string>

using namespace std;
using namespace logger;
using namespace state;

class LoggerMock : public ILogger {
  public:
	MOCK_METHOD1(LogState, void(IState *));
	MOCK_METHOD2(LogInstructionCount, void(PlayerId, int64_t));
	MOCK_METHOD3(LogError, void(PlayerId, int64_t, string));
	MOCK_METHOD0(LogFinalGameParams, void());
	MOCK_METHOD1(WriteGame, void(std::ostream &));
};

#endif
