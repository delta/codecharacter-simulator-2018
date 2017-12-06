/**
 * @file attack_state.cpp
 * Defines the soldier attack state
 */

#include "state/actor/soldier_states/attack_state.h"
#include "state/actor/soldier.h"
#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier_states/move_state.h"
#include "state/actor/soldier_states/pursuit_state.h"

namespace state {

AttackState::AttackState(Soldier *soldier)
    : SoldierState(SoldierStateName::ATTACK, soldier) {}

void AttackState::Enter() {}

std::unique_ptr<SoldierState> AttackState::Update() {
	// Check if the soldier is dead
	if (soldier->GetHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<DeadState>(soldier);
	}

	// Check if the destination is set
	if (soldier->IsDestinationSet()) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<MoveState>(soldier);
	}

	// Check if the target is dead
	if (soldier->GetAttackTarget()->GetHp() == 0) {
		soldier->SetAttackTarget(nullptr);
		return std::make_unique<IdleState>(soldier);
	}

	// Check if the target is out of range
	if (not soldier->IsAttackTargetInRange()) {
		return std::make_unique<PursuitState>(soldier);
	}

	// Execute attack code
	// Inflict damage on opponent
	int64_t soldier_attack_damage = soldier->GetAttackDamage();
	soldier->GetAttackTarget()->SetHp(std::max(
	    (long)0, soldier->GetAttackTarget()->GetHp() - soldier_attack_damage));

	return nullptr;
}

void AttackState::Exit() {}
}
