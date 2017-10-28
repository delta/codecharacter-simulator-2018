/**
 * @file soldier.h
 * Declares the Base Soldier class for each troop
 */

#ifndef STATE_ACTOR_SOLDIER_H
#define STATE_ACTOR_SOLDIER_H

#include "physics/vector.h"
#include "state/actor/actor.h"
#include "state/actor/respawn_system/respawn_system.h"
#include "state/state_export.h"
#include <cstdint>

namespace state {

/**
 * Soldier class that defines properties of a single soldier
 */
class STATE_EXPORT Soldier : public Actor {
  protected:
	/**
	 * Maximum possible soldier speed
	 */
	int64_t max_speed;

	/**
	 * Current soldier velocity
	 */
	physics::Vector velocity;

	/**
	 * Respawn System member for soldier
	 */
	RespawnSystem respawn_system;

  public:
	/**
	 * Soldier Constructor
	 */
	Soldier();

	Soldier(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector position, int64_t max_speed,
	        physics::Vector velocity);

	/**
	 * Get the soldier's current velocity
	 *
	 * @return     Soldier Velocity
	 */
	physics::Vector GetVelocity();

	/**
	 * Get the soldier's max speed stat
	 *
	 * @return     Soldier MaxSpeed
	 */
	int64_t GetMaxSpeed();

	/**
	 * Update function of the soldier
	 */
	void Update() override;
};
}

#endif
