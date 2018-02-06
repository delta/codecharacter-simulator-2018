/**
 * @file soldier_static_init.cpp
 * Sets the constant static members of the Soldier class
 */

#include "physics/vector.h"
#include "state/actor/soldier.h"

namespace state {

int64_t Soldier::total_turns_to_respawn = 10;
std::vector<physics::Vector> Soldier::respawn_positions(2, physics::Vector(40,
                                                                           40));
int64_t Soldier::total_num_turns_invulnerable = 2;
}
