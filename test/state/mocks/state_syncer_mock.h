#ifndef TEST_STATE_MOCKS_STATE_SYNCER_MOCK_H
#define TEST_STATE_MOCKS_STATE_SYNCER_MOCK_H

#include "gmock/gmock.h"
#include "state/interfaces/i_state_syncer.h"

class StateSyncerMock : public state::IStateSyncer {
  public:
	MOCK_METHOD2(ExecutePlayerCommands,
	             void(const std::vector<state::PlayerState *> &player_states,
	                  const std::vector<bool> &skip_player_commands_flags));

	MOCK_METHOD0(UpdateMainState, std::vector<int64_t>(void));

	MOCK_METHOD1(UpdatePlayerStates,
	             void(std::vector<state::PlayerState *> &player_states));
};

#endif
