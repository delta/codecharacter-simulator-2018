/**
 * @file attack_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/attack_state.h"
#include "state/actor/soldier.h"

namespace state {

AttackState::AttackState(Soldier *soldier)
    : SoldierState(SoldierStateName::ATTACK, soldier) {}

void AttackState::Enter() {}

std::unique_ptr<SoldierState> AttackState::Update() {
	// TODO: Define function
	return nullptr;
}

void AttackState::Exit() {}
}
