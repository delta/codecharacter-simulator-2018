/**
 * @file move_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/move_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/attack_state.h"
#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier_states/pursuit_state.h"

namespace state {

MoveState::MoveState(Soldier *soldier)
    : SoldierState(SoldierStateName::MOVE, soldier) {}

void MoveState::Enter() {}

std::unique_ptr<SoldierState> MoveState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		return std::make_unique<DeadState>(soldier);
	}

	// Check if there's an attack target set
	if (soldier->IsAttackTargetSet()) {
		if (not soldier->IsAttackTargetInRange()) {
			return std::make_unique<PursuitState>(soldier);
		} else {
			return std::make_unique<AttackState>(soldier);
		}
	}

	// Check if destination has been reached
	if (soldier->GetPosition() == soldier->GetDestination()) {
		return std::make_unique<IdleState>(soldier);
	}

	// Execute State Code
	physics::Vector source = soldier->GetPosition();
	physics::Vector dest = soldier->GetDestination();
	int64_t speed = soldier->GetSpeed();

	// Use path planner to get next position
	physics::Vector final_position =
	    soldier->GetPathPlanner()->GetNextPosition(source, dest, speed);

	// Set the new position
	soldier->SetNewPosition(final_position);
	return nullptr;
}

void MoveState::Exit() {
	// Unset the destination on exit
	soldier->ClearDestination();
}
}
