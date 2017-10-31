/**
 * @file soldier.h
 * Declares the Base Soldier class for each troop
 */

#ifndef STATE_ACTOR_SOLDIER_H
#define STATE_ACTOR_SOLDIER_H

#include "physics/vector.h"
#include "state/actor/actor.h"
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
	 * Distance in units around the soldier where actors can be attacked
	 */
	int64_t attack_range;

	/**
	 * Damage to inflict in one attack
	 */
	int64_t attack_damage;

	/**
	 * Actor that this soldier targets to attack
	 * nullptr if unset
	 */
	Actor *attack_target;

	/**
	 * Destination that the soldier needs to navigate to
	 * Valid only if is_destination_set is true
	 */
	physics::Vector destination;

	/**
	 * true if the Soldier is currently moving to the set destination
	 * false otherwise
	 */
	bool is_destination_set;

  public:
	/**
	 * Soldier Constructor
	 */
	Soldier();

	Soldier(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector position, int64_t speed,
	        int64_t attack_range, int64_t attack_damage);

	/**
	 * Position where the actors will respawn
	 */
	static physics::Vector respawn_position;

	/**
	 * Number of turns to wait before respawning
	 */
	static int64_t total_turns_to_respawn;

	/**
	 * Get the soldier's speed stat
	 *
	 * @return     Soldier Speed
	 */
	int64_t GetSpeed();

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
	 * Get the soldier's attack target
	 *
	 * @return     attack_target  Soldier's Attack Target
	 */
	Actor *GetAttackTarget();

	/**
	 * Set the soldier's attack target
	 *
	 * @param[in]  attack_target  Soldier's Attack Target
	 */
	void SetAttackTarget(Actor *attack_target);

	/**
	 * Check if the attack target is in range
	 *
	 * @return  true if target is in range, false otherwise
	 */
	bool IsAttackTargetInRange();

	/**
	 * Get the soldier's destination
	 *
	 * @return     Soldier's Destination
	 */
	physics::Vector GetDestination();

	/**
	 * Set the soldier's destination
	 *
	 * @param[in]  Soldier's Destination
	 */
	void SetDestination(physics::Vector destination);

	/**
	 * Check if the destination parameter is set
	 *
	 * @return     the destination_set field
	 */
	bool IsDestinationSet();

	/**
	 * Clear the current destination
	 * Set destination_set to false
	 */
	void ClearDestination();

	/**
	 * Check if the attack target is set
	 *
	 * @return     the attack_target field
	 */
	bool IsAttackTargetSet();

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
