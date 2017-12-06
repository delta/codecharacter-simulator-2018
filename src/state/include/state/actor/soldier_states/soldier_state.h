/**
 * @file soldier_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/soldier.fwd.h"
#include "state/state_export.h"
#include <memory>

#ifndef STATE_ACTOR_SOLDIER_STATES_SOLDIER_STATE_H
#define STATE_ACTOR_SOLDIER_STATES_SOLDIER_STATE_H

namespace state {

/**
 * Define a name for each state
 */
enum class SoldierStateName {
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

/**
 * The base state that other SoldierStates inherit from
 */
class STATE_EXPORT SoldierState {
  protected:
	/**
	 * Name of this state
	 */
	SoldierStateName state_name;

	/**
	 * Soldier that this state is managing
	 */
	Soldier *soldier;

  public:
	/**
	 * Constructor for SoldierState
	 */
	SoldierState(SoldierStateName state_name, Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 */
	virtual void Enter() = 0;

	/**
	 * Executes state code when called
	 * Returns the next soldier state when a state transition occurs
	 * Returns nullptr if state code has been executed (no transition)
	 *
	 * @return      A pointer to the new state
	 */
	virtual std::unique_ptr<SoldierState> Update() = 0;

	/**
	 * Called before the Soldier switches to another state
	 */
	virtual void Exit() = 0;

	/**
	 * Get this state's name
	 */
	SoldierStateName GetName();
};
}
#endif
