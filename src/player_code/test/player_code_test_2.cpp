#include "player_code/test/player_code_test_2.h"

namespace player_code {

player_state::State PlayerCode2::Update(player_state::State state) {
	std::string s(10E4, '*');
	logr << s << std::endl;
	return state;
}
}
