/**
 * @file soldier.h
 * Declares the Base Soldier class for each troop
 */

#ifndef STATE_ACTOR_SOLDIER_H
#define STATE_ACTOR_SOLDIER_H

#include "physics/vector.h"
#include "state/actor/actor.h"
#include "state/actor/mobility_system/mobility_system.h"
#include "state/actor/respawn_system/respawn_system.h"
#include "state/actor/soldier_state.h"
#include "state/map/interfaces/i_map.h"
#include "state/path_planner/path_planner.h"
#include "state/state_export.h"
#include <cstdint>

namespace state {

/**
 * Soldier class that defines properties of a single soldier
 */
class STATE_EXPORT Soldier : public Actor {
  protected:
	/**
	 * Soldier speed, in map-units/turn
	 */
	int64_t speed;

	/**
	 * Respawn System member for soldier
	 * Handles soldier respawn after death
	 */
	std::unique_ptr<RespawnSystem> respawn_system;

	/**
	 * Mobility System member for soldier
	 * Handles soldier movement and attacks
	 */
	std::unique_ptr<MobilitySystem> mobility_system;

	/**
	 * State of the soldier
	 */
	SoldierState soldier_state;

	/**
	 * Distance in units around the soldier where actors can be attacked
	 */
	int64_t attack_range;

	/**
	 * Damage to inflict in one attack
	 */
	int64_t attack_damage;

  public:
	/**
	 * Soldier Constructor
	 */
	Soldier();

	Soldier(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector position, int64_t speed,
	        SoldierState soldier_state, int64_t attack_range,
	        int64_t attack_damage, IPathPlanner *path_planner, IMap *map);

	/**
	 * Move soldier to a position
	 * Wrapper function over MobilitySystem
	 */
	void Move(physics::Vector Position);

	/**
	 * Attack a particular actor
	 * Wrapper function over MobilitySystem
	 */
	void Attack(Actor *attack_target);

	/**
	 * Pointer to Path Planner instance for movement calculations
	 */
	IPathPlanner *path_planner;

	/**
	 * Get the soldier's speed stat
	 *
	 * @return     Soldier Speed
	 */
	int64_t GetSpeed();

	/**
	 * Get the soldier's current state
	 *
	 * @return     Soldier's state
	 */
	SoldierState GetState();

	/**
	 * Get the soldier's attack range
	 *
	 * @return     Soldier's Attack Range
	 */
	int64_t GetAttackRange();

	/**
	 * Get the soldier's attack damage
	 *
	 * @return     Soldier's Attack Damage
	 */
	int64_t GetAttackDamage();

	/**
	 * Set the soldier's state
	 *
	 * @param[in]  Soldier's new state
	 */
	void SetState(SoldierState soldier_state);

	/**
	 * Set the soldier's position
	 *
	 * @param[in]  Soldier's new position
	 */
	void SetPosition(physics::Vector position);

	/**
	 * Update function of the soldier
	 */
	void Update() override;
};
}

#endif
