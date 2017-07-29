/**
 * @file respawn_system.h
 * Declares the methods that handle troop respawn
 */

#ifndef STATE_ACTOR_RESPAWN_SYSTEM_RESPAWN_SYSTEM_H
#define STATE_ACTOR_RESPAWN_SYSTEM_RESPAWN_SYSTEM_H

#include <cstdint>
#include "physics/vector.h"
#include "state/interfaces/i_updatable.h"
#include "state/actor/actor.h"
#include "state/actor/soldier.fwd.h"
#include "state/state_export.h"

namespace state {

/**
 * Respawn System class that adds respawn functionality to troops
 */
class STATE_EXPORT RespawnSystem : public IUpdatable {
private:

	/**
	 * Pointer to actor that this respawn system controls
	 */
	Soldier* soldier;

	/**
	 * Number of remaining turns this actor must wait to respawn
	 */
	int64_t remaining_turns_to_respawn;

	/**
	 * True if the timer is currently active
	 * False if actor is alive and well
	 */
	bool timer_active;

	/**
	 * True when the actor is ready to respawn
	 * False otherwise. Useful for the soldier to know when to respawn
	 */
	bool is_ready_to_respawn;

public:
	/**
	 * Public Member -
	 * Position where the actors will respawn
	 */
	static physics::Vector respawn_position;

	/**
	 * Number of turns to wait before respawning
	 */
	static int64_t total_turns_to_respawn;

	/**
	 * Constructors
	 */
	RespawnSystem();
	RespawnSystem(
		Soldier* soldier
	);

	~RespawnSystem();

	/**
	 * Starts the respawn timer countdown
	 */
	void StartRespawnTimer();

	/**
	 * @brief      Gets if the actor is ready to respawn
	 *
	 * @return     The current respawn status
	 */
	bool IsReadyToRespawn();

	/**
	 * Update method of the respawn system
	 */
	void Update() override;

};

}

#endif
