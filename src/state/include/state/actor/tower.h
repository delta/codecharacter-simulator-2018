/**
 * @file tower.h
 * Declares the Base Tower class
 */

#ifndef STATE_ACTOR_TOWER_H
#define STATE_ACTOR_TOWER_H

#include "physics/vector.h"
#include "state/actor/actor.h"
#include "state/state_export.h"
#include "state/utilities.h"
#include <cstdint>

namespace state {

/**
 * Integer type for representing a tower's upgrade level
 */
typedef int64_t TowerLevel;

/**
 * Tower class that defines a tower's properties
 */
class STATE_EXPORT Tower : public Actor {
  protected:
	/**
	 * If true, the tower is the player's home base tower
	 */
	bool is_base;

	/**
	 * Upgrade level of the tower
	 */
	TowerLevel tower_level;

  public:
	/**
	 * Tower Constructor
	 */
	Tower();

	Tower(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	      int64_t max_hp, physics::Vector position, bool is_base,
	      TowerLevel tower_level);

	/**
	 * Get if the tower is a base tower
	 *
	 * @return     Is tower base
	 */
	bool GetIsBase();

	/**
	 * Get the tower's current upgrade level
	 *
	 * @return     Current tower level
	 */
	TowerLevel GetTowerLevel();

	/**
	 * Upgrades the tower, increments tower level
	 *
	 * @param[in]  new_hp  New max_hp and hp to set
	 */
	void Upgrade(int64_t new_hp);

	/**
	 * Update function of the tower
	 */
	void Update() override;
};
}

#endif
