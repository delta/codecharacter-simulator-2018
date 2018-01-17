#include "player_code/test/player_code_test_1.h"
#include <algorithm>
#include <vector>

namespace player_code {

PlayerCode1::PlayerCode1(state::PlayerState *player_state)
    : player_state(player_state) {}

void PlayerCode1::Update() {
	std::vector<int> v(10000, 5);
	for (int i = 0; i < v.size(); ++i)
		v[i] = v.size() - i;
	std::sort(v.begin(), v.end());
}
}
