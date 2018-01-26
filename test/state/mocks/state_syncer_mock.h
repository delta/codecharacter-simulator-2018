#ifndef TEST_STATE_MOCKS_STATE_SYNCER_MOCK_H
#define TEST_STATE_MOCKS_STATE_SYNCER_MOCK_H

#include "gmock/gmock.h"
#include "state/interfaces/i_state_syncer.h"

class StateSyncerMock : public state::IStateSyncer {
  public:
	MOCK_METHOD2(ExecutePlayerCommands,
	             void(const std::vector<player_state::State *> &player_states,
	                  const std::vector<bool> &skip_player_commands_flags));

	MOCK_METHOD0(UpdateMainState, void());

	MOCK_METHOD1(UpdatePlayerStates,
	             void(std::vector<player_state::State *> &player_states));

	MOCK_METHOD0(GetScores, std::vector<int64_t>());
};

#endif
