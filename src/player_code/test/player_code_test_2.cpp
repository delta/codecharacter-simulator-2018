#include "player_code/test/player_code_test_2.h"

namespace player_code {

PlayerCode2::PlayerCode2(state::PlayerState *player_state)
    : player_state(player_state) {}

void PlayerCode2::Update() {
	std::string s(10E4, '*');
	logr << s << std::endl;
}
}
