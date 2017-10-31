/**
 * @file soldier_state.h
 * Declares a utility enum for states of a soldier
 */

#ifndef STATE_ACTOR_SOLDIER_STATE_H
#define STATE_ACTOR_SOLDIER_STATE_H

namespace state {

/**
 * Type of state that the soldier is active in
 */
enum class SoldierState {
	// Soldier is doing nothing
	IDLE,
	// Soldier is in movement nothing
	MOVE,
	// Soldier is currently attacking another actor
	ATTACK,
	// Soldier is in the process of respawning
	DEAD
};
}

#endif
