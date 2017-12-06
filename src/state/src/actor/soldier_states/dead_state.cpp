/**
 * @file dead_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/dead_state.h"
#include "state/actor/soldier.h"

namespace state {

DeadState::DeadState(Soldier *soldier)
    : SoldierState(SoldierStateName::DEAD, soldier) {}

void DeadState::Enter() {}

std::unique_ptr<SoldierState> DeadState::Update() {
	// TODO: Define function
	return nullptr;
}

void DeadState::Exit() {}
}
