/**
 * @file move_state.cpp
 * Defines the soldier idle state
 */

#include "state/actor/soldier_states/move_state.h"
#include "state/actor/soldier.h"

namespace state {

MoveState::MoveState(Soldier *soldier)
    : SoldierState(SoldierStateName::MOVE, soldier) {}

void MoveState::Enter() {}

std::unique_ptr<SoldierState> MoveState::Update() {
	// TODO: Define function
	return nullptr;
}

void MoveState::Exit() {}
}
