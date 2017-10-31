/**
 * @file soldier_static_init.cpp
 * Sets the constant static members of the Soldier class
 */

#include "physics/vector.h"
#include "state/actor/soldier.h"

namespace state {

int64_t Soldier::total_turns_to_respawn = 10;
physics::Vector Soldier::respawn_position = physics::Vector(40, 40);
}
