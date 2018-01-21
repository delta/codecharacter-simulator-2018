/**
 * @file player_state.h
 * Player's copy of state
 */

#ifndef STATE_PLAYER_STATE_H
#define STATE_PLAYER_STATE_H

#include "constants/constants.h"
#include "physics/vector.h"
#include "state/actor/soldier_states/soldier_state.h"
#include "state/map/terrain_type.h"
#include "state/state_export.h"
#include "state/utilities.h"
#include <array>

namespace state {

// The members of each struct marked 'Writable' can be written to
// by the player. They carry information about the player's current move

/**
 * Struct holding information about each map grid element
 */
struct PlayerMapElement {
	TerrainType terrain;
	std::array<bool, static_cast<std::size_t>(PlayerId::PLAYER_COUNT)>
	    territory;
	bool valid_territory;

	// Writable
	bool build_tower;
};

/**
 * Struct holding information about a particular soldier
 */
struct PlayerSoldier {
	int64_t id;
	physics::Vector position;
	int64_t hp;
	SoldierStateName state;

	// Writable
	int64_t tower_target;
	int64_t soldier_target;
	physics::Vector destination;
};

/**
 * Struct holding information about a particular tower
 */
struct PlayerTower {
	int64_t id;
	physics::Vector position;
	int64_t hp;
	int64_t level;

	// Writable
	bool upgrade_tower;
	bool suicide;
};

/**
 * Struct holding information about Opponent's soldier
 */
struct OpponentSoldier {
	int64_t id;
	physics::Vector position;
	int64_t hp;
	SoldierStateName state;
};

/**
 * Struct holding information about Opponent's tower
 */
struct OpponentTower {
	int64_t id;
	physics::Vector position;
	int64_t hp;
	int64_t level;
};

/**
 * Player's copy of state
 */
struct STATE_EXPORT PlayerState {
	// Grid of map elements
	std::array<std::array<PlayerMapElement, MAP_SIZE>, MAP_SIZE> map;

	// List of player soldiers
	std::array<PlayerSoldier, NUM_SOLDIERS> soldiers;

	// List of opponent soldiers
	std::array<PlayerSoldier, NUM_SOLDIERS> opponent_soldiers;

	// List of player towers
	std::array<PlayerTower, MAX_NUM_TOWERS> towers;

	// List of opponent soldiers
	std::array<PlayerTower, MAX_NUM_TOWERS> opponent_towers;

	// Number of player towers
	int64_t num_towers;

	// Number of opponent towers
	int64_t num_opponent_towers;

	// Money
	int64_t money;
};
}

#endif
