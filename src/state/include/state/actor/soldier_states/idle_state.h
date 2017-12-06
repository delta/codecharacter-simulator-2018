/**
 * @file idle_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#ifndef STATE_ACTOR_SOLDIER_STATES_IDLE_STATE_H
#define STATE_ACTOR_SOLDIER_STATES_IDLE_STATE_H

namespace state {

/**
 * The idle soldier state class
 */
class STATE_EXPORT IdleState : public SoldierState {
  public:
	IdleState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear any attack target or destination
	 */
	void Enter();

	/**
	 * Performs state transitions
	 *
	 * If soldier is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If there's an attack target in range, switch to attack state
	 * If it's not in range, switch to pursuit state
	 * Else, remain in idle state. Do nothing
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<SoldierState> Update();

	/**
	 * Called before the Soldier switches to another state
	 */
	void Exit();
};
}
#endif
