#include "player_code/test/player_code_test_0.h"

namespace player_code {

player_state::State PlayerCode0::Update(player_state::State state) {
	int a = 0;
	for (unsigned i = 0; i < 10E5; ++i) {
		a = a + i;
		a = a - i;
	}
	return state;
}
}
