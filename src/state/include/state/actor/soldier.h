/**
 * @file soldier.h
 * Declares the Base Soldier class for each troop
 */

#ifndef STATE_ACTOR_SOLDIER_H
#define STATE_ACTOR_SOLDIER_H

#include "physics/vector.h"
#include "state/actor/actor.h"
#include "state/actor/soldier_states/soldier_state.h"
#include "state/interfaces/i_path_planner.h"
#include "state/money_manager/money_manager.h"
#include "state/state_export.h"
#include <cstdint>
#include <vector>

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

	/**
	 * Position where the soldier should move to, if the soldier must move
	 * Serves as a temp for updating soldier positions
	 */
	physics::Vector new_position;

	/**
	 * true if new_position is set
	 * false otherwise
	 */
	bool is_new_position_set;

	/*
	 * State class that controls logic for soldier's current state
	 */
	std::unique_ptr<SoldierState> state;

	/**
	 * Pointer to PathPlanner instance
	 */
	IPathPlanner *path_planner;

	/**
	 * Pointer to MoneyManager instance
	 */
	MoneyManager *money_manager;

  public:
	/**
	 * Soldier Constructor
	 */
	Soldier();

	Soldier(ActorId id, PlayerId player_id, ActorType actor_type, int64_t hp,
	        int64_t max_hp, physics::Vector position, int64_t speed,
	        int64_t attack_range, int64_t attack_damage,
	        IPathPlanner *path_planner, MoneyManager *money_manager);

	/**
	 * Position where the actors will respawn, indexed by player ID
	 */
	static std::vector<physics::Vector> respawn_positions;

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
	 * Get the soldier's new_position
	 *
	 * @return     Soldier's new_position
	 */
	physics::Vector GetNewPosition();

	/**
	 * Set the soldier's new_position
	 *
	 * @param[in]  Soldier's new_position
	 */
	void SetNewPosition(physics::Vector new_position);

	/**
	 * Check if the new_position parameter is set
	 *
	 * @return     the is_new_position_set field
	 */
	bool IsNewPositionSet();

	/**
	 * Clear the current new_position
	 * Set is_new_position_set to false
	 */
	void ClearNewPosition();

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
	 * Get the pointer to the path planner
	 *
	 * @return     Pointer to PathPlanner instance
	 */
	IPathPlanner *GetPathPlanner();

	/**
	 * Get the pointer to the money manager
	 *
	 * @return     Pointer to MoneyManager instance
	 */
	MoneyManager *GetMoneyManager();

	/**
	 * Get the name of the current state
	 *
	 * @return     Name of current state of type SoldierStateName
	 */
	SoldierStateName GetState();

	/**
	 * Method to give the move command to this soldier
	 *
	 * @param[in]     destination    The destination to go to
	 */
	void Move(physics::Vector destination);

	/**
	 * Method to give the attack command to this soldier
	 *
	 * @param[in]     attack_target  The target to attack
	 */
	void Attack(Actor *attack_target);

	/**
	 * Executes code that should be run only after all other actor updates
	 */
	void LateUpdate();

	/**
	 * Update function of the soldier
	 */
	void Update() override;
};
}

#endif
