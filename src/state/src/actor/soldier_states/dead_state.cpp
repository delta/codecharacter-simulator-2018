/**
 * @file dead_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/idle_state.h"

namespace state {

DeadState::DeadState(Soldier *soldier)
    : SoldierState(SoldierStateName::DEAD, soldier),
      remaining_turns_to_respawn(0) {}

void DeadState::Enter() {
	// Start the respawn timer
	this->remaining_turns_to_respawn = Soldier::total_turns_to_respawn;
}

std::unique_ptr<SoldierState> DeadState::Update() {
	// Check if soldier should be respawned
	if (remaining_turns_to_respawn == 0) {
		return std::make_unique<IdleState>(soldier);
	}

	// Decrement respawn timer
	remaining_turns_to_respawn--;

	return nullptr;
}

void DeadState::Exit() {
	soldier->SetPosition(Soldier::respawn_position);
	soldier->SetHp(soldier->GetMaxHp());
	soldier->SetAttackTarget(nullptr);
	soldier->ClearDestination();
}
}
