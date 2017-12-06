/**
 * @file attack_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#ifndef STATE_ACTOR_SOLDIER_STATES_ATTACK_STATE_H
#define STATE_ACTOR_SOLDIER_STATES_ATTACK_STATE_H

namespace state {

/**
 * The attack soldier state class
 */
class STATE_EXPORT AttackState : public SoldierState {
  public:
	AttackState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear destination
	 */
	void Enter();

	/**
	 * Performs state transitions
	 *
	 * If soldier is dead, switch to dead state
	 * If there's a destination set, switch to move state
	 * If there target is dead, switch to idle state
	 * If the target is out of range, switch to pursuit state
	 * Else, remain in attack state. Inflict damage on the target
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
