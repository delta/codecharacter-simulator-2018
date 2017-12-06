/**
 * @file pursuit_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#ifndef STATE_ACTOR_SOLDIER_STATES_PURSUIT_STATE_H
#define STATE_ACTOR_SOLDIER_STATES_PURSUIT_STATE_H

namespace state {

/**
 * The pursuit soldier state class
 */
class STATE_EXPORT PursuitState : public SoldierState {
  public:
	PursuitState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 */
	void Enter();

	/**
	 * Executes state code when called
	 * Returns the next soldier state
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
