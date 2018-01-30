#include "player_code/test/player_code_test_1.h"
#include <algorithm>
#include <vector>

namespace player_code {

player_state::State PlayerCode1::Update(player_state::State state) {
	std::vector<int> v(10000, 5);
	for (int i = 0; i < v.size(); ++i)
		v[i] = v.size() - i;
	std::sort(v.begin(), v.end());
	return state;
}
}
