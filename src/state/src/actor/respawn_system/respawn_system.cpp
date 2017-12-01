/**
 * @file respawn_system.cpp
 * Defines the methods that handle troop respawn
 */

#include "state/actor/respawn_system/respawn_system.h"
#include "physics/vector.h"
#include "state/actor/soldier.h"

namespace state {

RespawnSystem::~RespawnSystem() {}

RespawnSystem::RespawnSystem() {
	// Init None
}

RespawnSystem::RespawnSystem(Soldier *soldier)
    : soldier(soldier), remaining_turns_to_respawn(0), timer_active(false),
      is_ready_to_respawn(false) {}

void RespawnSystem::StartRespawnTimer() {
	this->remaining_turns_to_respawn = RespawnSystem::total_turns_to_respawn;

	this->timer_active = true;
}

bool RespawnSystem::IsReadyToRespawn() { return this->is_ready_to_respawn; }

void RespawnSystem::Update() {
	// The actor shouldn't be ready to
	// respawn unless set for that turn
	if (is_ready_to_respawn) {
		is_ready_to_respawn = false;
	}

	// Check if the Actor is in the respawn process
	if (timer_active) {
		remaining_turns_to_respawn--;

		// Check if it should be respawned
		if (remaining_turns_to_respawn == 0) {
			// Respawn the actor
			is_ready_to_respawn = true;
			timer_active = false;
		}

	} else if (this->soldier->GetHp() == 0) {
		this->soldier->SetState(SoldierState::DEAD);
		StartRespawnTimer();
	}
}
}
