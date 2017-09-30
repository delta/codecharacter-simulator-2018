/**
 * @file player_state.h
 * Player's copy of state
 */

#ifndef STATE_PLAYER_STATE_H
#define STATE_PLAYER_STATE_H

#include <vector>
#include "physics/vector.h"
#include "state/map/terrain_type.h"
#include "state/state_export.h"

namespace state {

struct PlayerMapElement {
	TerrainType terrain;
	physics::Vector position;
	std::vector<bool> territory;

	// Writable
	bool build_tower;
};

struct PlayerSoldier {
	int64_t id;
	physics::Vector position;
	int64_t hp;

	// Writable
	int64_t tower_target;
	int64_t soldier_target;
	physics::Vector destination;
};

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
 * Player's copy of state
 */
struct STATE_EXPORT PlayerState {
	// Grid of map elements
	std::vector<std::vector<PlayerMapElement> > map;

	// Lists of soldiers, indexed by player_id
	std::vector<std::vector<PlayerSoldier> > soldiers;

	// Lists of towers, indexed by player_id
	std::vector<std::vector<PlayerTower> > towers;
};

}

#endif
