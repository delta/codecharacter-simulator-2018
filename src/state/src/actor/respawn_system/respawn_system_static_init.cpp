/**
 * @file respawn_system_static_init.cpp
 * Sets the constant static members of the RespawnSystem class
 */

#include "physics/vector.h"
#include "state/actor/respawn_system/respawn_system.h"

namespace state {

int64_t RespawnSystem::total_turns_to_respawn = 10;
physics::Vector RespawnSystem::respawn_position = physics::Vector(40, 40);
}