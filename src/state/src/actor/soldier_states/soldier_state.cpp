/**
 * @file soldier_state.cpp
 * Defines the base state that SoldierStates inherit from
 */

#include "state/actor/soldier_states/soldier_state.h"
#include "state/actor/soldier.h"

namespace state {

SoldierState::SoldierState(SoldierStateName state_name, Soldier *soldier)
    : state_name(state_name), soldier(soldier) {}

SoldierStateName SoldierState::GetName() { return state_name; }
}
