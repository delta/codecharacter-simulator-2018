#include "player_code/player_code.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace player_code {

using namespace player_state;

int turn = 0;
bool save_up = false;

int impairment = 80; // Higher means AI is weaker

void PrintMap(player_state::State &state) {
	for (auto &row : state.map) {
		for (auto &elt : row) {
			std::cout << elt.valid_territory;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl << std::endl;
}

State PlayerCode::Update(State state) {
	// for (int i = 0; i < 10E5; ++i);
	// std::vector<int> v(10000, 5);
	// for (int i = 0; i < v.size(); ++i)
	// 	v[i] = v.size() - i;
	// std::sort(v.begin(), v.end());
	turn++;
	if (turn % 50 == 0)
		save_up = true;
	for (int i = 0; i < state.soldiers.size() / 2; ++i) {
		auto &soldier = state.soldiers[i];
		if (soldier.hp == 0)
			continue;
		for (auto enemy_soldier : state.enemy_soldiers) {
			if (enemy_soldier.hp > 0) {
				soldier.soldier_target = enemy_soldier.id;
				break;
			}
		}
	}

	for (int i = state.soldiers.size() / 2; i < state.soldiers.size(); ++i) {
		auto &soldier = state.soldiers[i];
		if (soldier.hp == 0)
			continue;
		if (state.num_enemy_towers > 1)
			soldier.tower_target = state.enemy_towers[1].id;
	}

	int num_max_towers = 0;
	// std::cout << state.num_towers << std::endl;
	for (int i = 0; i < state.num_towers; ++i) {
		auto &tower = state.towers[i];
		// std::cout << tower.level << " " << state.money << std::endl;;
		if (tower.level < TOWER_BUILD_COSTS.size()) {
			if (state.money >= TOWER_BUILD_COSTS[tower.level]) {
				tower.upgrade_tower = true;
				state.money -= TOWER_BUILD_COSTS[tower.level];
				save_up = false;
				break;
			}
		} else {
			num_max_towers++;
		}
	}
	if (num_max_towers == state.num_towers)
		save_up = false;

	if (save_up || state.num_towers == MAX_NUM_TOWERS)
		return state;

	int64_t best_territory_count = 0;
	physics::Vector best_offset;
	// for (int i = MAP_SIZE - 1; i >= 0; --i) {
	for (int i = 0; i < MAP_SIZE; ++i) {
		auto &row = state.map[i];
		// for (int j = MAP_SIZE - 1; j >= 0; --j) {
		for (int j = 0; j < MAP_SIZE; ++j) {
			auto &elt = row[j];
			bool can_build = true;
			physics::Vector offset(i, j);

			for (int k = 0; k < state.num_towers; ++k) {
				auto &tower = state.towers[k];
				if ((tower.position / MAP_ELEMENT_SIZE).floor() == offset) {
					can_build = false;
					break;
				}
			}

			int64_t cur_territory_count = 0;
			if (can_build && elt.valid_territory &&
			    state.money >= TOWER_BUILD_COSTS[0]) {
				for (int x = -TOWER_RANGES[0]; x <= TOWER_RANGES[0]; ++x) {
					for (int y = -TOWER_RANGES[0]; y <= TOWER_RANGES[0]; ++y) {
						if (offset.x + x >= 0 && offset.x + x < MAP_SIZE &&
						    offset.y + y >= 0 && offset.y + y < MAP_SIZE &&
						    state.map[offset.x + x][offset.y + y].territory ==
						        false) {
							cur_territory_count++;
						}
					}
				}
				if (cur_territory_count > best_territory_count) {
					best_territory_count = cur_territory_count;
					best_offset = offset;
				}
			}
		}
	}
	if (best_offset != physics::Vector(0, 0) && turn % impairment == 0) {
		state.map[best_offset.x][best_offset.y].build_tower = true;
	}
	return state;
}
}
