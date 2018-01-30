#include "player_code/player_code.h"

namespace player_code {

using namespace physics;
using namespace player_state;
// using namespace std; Feel free to uncomment this if you want :)

int cur_patrol_index = 0;

player_state::State PlayerCode::Update(player_state::State state) {
	// We're going to make our soldiers patrol our base tower
	auto base_pos = state.towers[0].position;

	// Setting route for patrolling
	std::vector<Vector> base_patrol_positions({
	    base_pos + Vector(0, -MAP_ELEMENT_SIZE * 3), // Top
	    base_pos + Vector(MAP_ELEMENT_SIZE * 3, 0),  // Right
	    base_pos + Vector(0, MAP_ELEMENT_SIZE * 3),  // Bottom
	    base_pos + Vector(-MAP_ELEMENT_SIZE * 3, 0)  // Left
	});

	// If the first soldier is nearly at the patrol spot, start moving to the
	// next one
	auto leader = state.soldiers[0];
	if (leader.position.distance(base_patrol_positions[cur_patrol_index]) <
	    MAP_ELEMENT_SIZE) {
		cur_patrol_index =
		    (cur_patrol_index + 1) % base_patrol_positions.size();
	}

	// Make the first half of the soldiers patrol
	for (int i = 0; i < NUM_SOLDIERS / 2; ++i) {
		auto &soldier = state.soldiers[i];
		if (soldier.hp != 0) // Ensure we don't give orders to dead soldiers
			soldier.destination = base_patrol_positions[cur_patrol_index];
	}

	// Log the current patrol destination
	logr << "Current patrol index: " << cur_patrol_index << std::endl;

	// Done with patrolling
	// Now for attacking

	// Make the soldiers who aren't patrolling attack the enemy
	for (int i = NUM_SOLDIERS / 2; i < NUM_SOLDIERS; ++i) {
		auto &soldier = state.soldiers[i];
		if (soldier.hp == 0) // If this soldier is dead, skip it
			continue;

		for (auto enemy_soldier : state.enemy_soldiers) {
			if (enemy_soldier.hp != 0) { // Ensure your prospective target has
				                         // not already been slain
				soldier.soldier_target = enemy_soldier.id;
				break;
			}
		}
	}

	// Done with attacking
	// Now for upgrading towers

	// We're going to upgrade our base tower, but only if we have enough money,
	// and if it's not already at the max level
	auto &base_tower = state.towers[0];
	auto upgrade_cost = TOWER_BUILD_COSTS[base_tower.level - 1];
	if (base_tower.level < MAX_TOWER_LEVEL && state.money >= upgrade_cost) {
		base_tower.upgrade_tower = true;
		state.money -= upgrade_cost;
	}

	// Done with tower upgrades
	// Now for building towers

	// We build one tower at the edge of our base tower's territory if we have
	// the money and that tile of the map is valid (we exclusively own it,
	// and don't already have a tower there)
	if (state.money >= TOWER_BUILD_COSTS[0]) {
		auto base_tower_range = TOWER_RANGES[base_tower.level - 1];
		auto build_pos = (base_pos / MAP_ELEMENT_SIZE).floor() +
		                 Vector(base_tower_range, base_tower_range);

		auto &map_elt = state.map[build_pos.x][build_pos.y];

		if (map_elt.valid_territory)
			state.map[build_pos.x][build_pos.y].build_tower = true;
	}

    // Return the modified state
    return state;

	// That's it
	// You may have noticed that this code is quite useless
	// The defending soldiers don't do anything, we only upgrade the base tower,
	// and only build one tower near the base.
	// So improve this code, or make your own completely new strategy
	// See you on the leaderboard!
}
}
