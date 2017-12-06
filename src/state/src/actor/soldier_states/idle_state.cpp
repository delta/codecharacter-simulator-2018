/**
 * @file idle_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/attack_state.h"
#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier_states/move_state.h"
#include "state/actor/soldier_states/pursuit_state.h"

namespace state {

IdleState::IdleState(Soldier *soldier)
    : SoldierState(SoldierStateName::IDLE, soldier) {}

void IdleState::Enter() {}

std::unique_ptr<SoldierState> IdleState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		return std::make_unique<DeadState>(soldier);
	}

	// Check if the destination is set
	if (soldier->IsDestinationSet()) {
		return std::make_unique<MoveState>(soldier);
	}

	// Check if there's an attack target set
	if (soldier->IsAttackTargetSet()) {
		if (not soldier->IsAttackTargetInRange()) {
			return std::make_unique<PursuitState>(soldier);
		} else {
			return std::make_unique<AttackState>(soldier);
		}
	}
	return nullptr;
}

void IdleState::Exit() {}
}
