#include "player_code/test/player_code_test_0.h"

namespace player_code {

PlayerCode0::PlayerCode0(player_state::State *player_state)
    : player_state(player_state) {}

void PlayerCode0::Update() {
	int a = 0;
	for (unsigned i = 0; i < 10E5; ++i) {
		a = a + i;
		a = a - i;
	}
}
}
