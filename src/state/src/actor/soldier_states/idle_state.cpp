/**
 * @file idle_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier.h"

namespace state {

IdleState::IdleState(Soldier *soldier)
    : SoldierState(SoldierStateName::IDLE, soldier) {}

void IdleState::Enter() {}

std::unique_ptr<SoldierState> IdleState::Update() {
	// TODO: Define function
	return nullptr;
}

void IdleState::Exit() {}
}
