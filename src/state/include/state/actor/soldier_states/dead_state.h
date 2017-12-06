/**
 * @file dead_state.h
 * Declares an interface for soldier states
 */

#include "state/actor/soldier.fwd.h"
#include "state/actor/soldier_states/soldier_state.h"

#ifndef STATE_ACTOR_SOLDIER_STATES_DEAD_STATE_H
#define STATE_ACTOR_SOLDIER_STATES_DEAD_STATE_H

namespace state {

/**
 * The dead soldier state class
 */
class STATE_EXPORT DeadState : public SoldierState {
  private:
	/**
	 * Number of remaining turns this soldier must wait to respawn
	 */
	int64_t remaining_turns_to_respawn;

  public:
	DeadState(Soldier *soldier);

	/**
	 * Called right after the soldier switches to this state
	 *
	 * Clear any attack target or destination, and start the respawn timer
	 */
	void Enter();

	/**
	 * Performs state transitions
	 *
	 * If respawn timer is complete, switch to idle state
	 * Else, remain in deadstate. Move towards the destination
	 *
	 * @return      A pointer to the new state
	 */
	std::unique_ptr<SoldierState> Update();

	/**
	 * Called before the Soldier switches to another state
	 *
	 * Reset soldier's stats
	 */
	void Exit();
};
}
#endif
