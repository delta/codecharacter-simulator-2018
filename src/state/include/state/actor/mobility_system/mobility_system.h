/**
 * @file mobility_system.h
 * Declares the Mobility System for soldier attacks and movement
 */

#ifndef STATE_MOBILITY_SYSTEM_MOBILITY_SYSTEM_H
#define STATE_MOBILITY_SYSTEM_MOBILITY_SYSTEM_H

#include "physics/vector.h"
#include "state/actor/actor.h"
#include "state/actor/soldier.fwd.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/interfaces/i_map.h"
#include "state/state_export.h"
#include <cstdint>

namespace state {

class STATE_EXPORT MobilitySystem : public IUpdatable {
  private:
	/**
	 * An actor to attack
	 */
	Actor *attack_target;

	/**
	 * True if attack_target is currently set
	 * False if not
	 */
	bool attack_target_set;

	/**
	 * A position to move towards
	 */
	physics::Vector destination;

	/**
	 * true if destination is currently set
	 * flase if not
	 */
	bool destination_set;

	/**
	 * true if the destination was changed in the last turn
	 * false if not
	 */
	bool destination_set_in_prev_turn;

	/**
	 * Soldier that this MobilitySystem belongs to
	 */
	Soldier *soldier;

	/**
	 * Pointer to the map, to get position information
	 */
	IMap *map;

	/**
	 * Utility function to check if soldier in attack range of the target
	 *
	 * @return true if attack_target is in range of the soldier
	 *         false otherwise
	 */
	bool IsAttackTargetInRange();

	/**
	 * Performs move procedure
	 */
	void HandleMove();

	/**
	 * Performs attack procedure
	 */
	void HandleAttack();

  public:
	MobilitySystem();
	MobilitySystem(Actor *attack_target, bool attack_target_set,
	               physics::Vector destination, bool destination_set,
	               bool destination_set_in_prev_turn, Soldier *soldier,
	               IMap *map);

	/**
	 * Sets the systems target and initiates attack if target in range
	 * Else, the Soldier is set to the move state with the target as
	 * the destination.
	 *
	 * @param[in]   attack_target   Actor (Tower, Soldier) to attack
	 */
	void AttackActor(Actor *attack_target);

	/**
	 * Sets the system's destination, and moves the soldier towards it
	 * Invokes the path planner if the destination is not within the
	 * current map grid.
	 *
	 * @param[in]   destination     Position to move to
	 */
	void MoveTo(physics::Vector destination);

	/**
	 * Updates MobilitySystem properties, damages target
	 */
	void Update();
};
}

#endif
