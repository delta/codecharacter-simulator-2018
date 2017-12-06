/**
 * @file pursuit_state.cpp
 * Defines the soldier pursuit state
 */

#include "state/actor/soldier_states/pursuit_state.h"
#include "state/actor/soldier.h"

namespace state {

PursuitState::PursuitState(Soldier *soldier)
    : SoldierState(SoldierStateName::PURSUIT, soldier) {}

void PursuitState::Enter() {}

std::unique_ptr<SoldierState> PursuitState::Update() {
	// TODO: Define function
	return nullptr;
}

void PursuitState::Exit() {}
}
