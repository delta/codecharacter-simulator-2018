/**
 * @file player_state.h
 * Player's copy of state
 */

#ifndef STATE_PLAYER_STATE_H
#define STATE_PLAYER_STATE_H

#include "constants/constants.h"
#include "physics/vector.h"
#include <array>
#include <ostream>

namespace player_state {

/**
 * Define a name for each soldier state
 */
enum class SoldierState {
	// Soldier is doing nothing
	IDLE,
	// Soldier is in movement
	MOVE,
	// Soldier is currently attacking another actor
	ATTACK,
	// Soldier is currently moving to attack another actor
	PURSUIT,
	// Soldier is in the process of respawning
	DEAD
};

inline std::ostream &operator<<(std::ostream &os, SoldierState soldier_state) {
	switch (soldier_state) {
	case SoldierState::IDLE:
		os << "IDLE";
		break;
	case SoldierState::ATTACK:
		os << "ATTACK";
		break;
	case SoldierState::PURSUIT:
		os << "PURSUIT";
		break;
	case SoldierState::MOVE:
		os << "MOVE";
		break;
	case SoldierState::DEAD:
		os << "DEAD";
		break;
	}
	return os;
}

// The members of each struct marked 'Writable' can be written to
// by the player. They carry information about the player's current move

/**
 * Struct holding information about each map grid element
 */
struct MapElement {
	bool territory;
	bool enemy_territory;
	bool valid_territory;

	// Writable
	bool build_tower;
};

/**
 * Struct holding information about a particular soldier
 */
struct Soldier {
	int64_t id;
	physics::Vector position;
	int64_t hp;
	SoldierState state;
	bool is_immune;

	// Writable
	int64_t tower_target;
	int64_t soldier_target;
	physics::Vector destination;
};

/**
 * Struct holding information about a particular tower
 */
struct Tower {
	int64_t id;
	physics::Vector position;
	int64_t hp;
	int64_t level;

	// Writable
	bool upgrade_tower;
	bool suicide;
};

/**
 * Player's copy of state
 */
struct State {
	// Grid of map elements
	std::array<std::array<MapElement, MAP_SIZE>, MAP_SIZE> map;

	// List of player soldiers
	std::array<Soldier, NUM_SOLDIERS> soldiers;

	// List of enemy soldiers
	std::array<Soldier, NUM_SOLDIERS> enemy_soldiers;

	// List of player towers
	std::array<Tower, MAX_NUM_TOWERS> towers;

	// List of enemy soldiers
	std::array<Tower, MAX_NUM_TOWERS> enemy_towers;

	// Number of player towers
	int64_t num_towers;

	// Number of enemy towers
	int64_t num_enemy_towers;

	// Money
	int64_t money;
};
}

#endif
