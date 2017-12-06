/**
 * @file move_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#ifndef STATE_ACTOR_SOLDIER_STATES_MOVE_STATE_H
#define STATE_ACTOR_SOLDIER_STATES_MOVE_STATE_H

namespace state {

/**
 * The move soldier state class
 */
class STATE_EXPORT MoveState : public SoldierState {
  public:
	MoveState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear attack target
	 */
	void Enter();

	/**
	 * Performs state transitions
	 *
	 * If soldier is dead, switch to dead state
	 * If there's a target in range, switch to attack state
	 * If there's a target out of range, switch to pursuit state
	 * If destination has been reached, switch to idle state
	 * Else, remain in move state. Move towards the destination
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<SoldierState> Update();

	/**
	 * Called before the Soldier switches to another state
	 *
	 * Clear destination
	 */
	void Exit();
};
}
#endif
