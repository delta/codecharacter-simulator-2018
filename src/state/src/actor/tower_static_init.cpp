/**
 * @file tower_static_init.cpp
 * Sets the constant static members of the Tower class
 */

#include "physics/vector.h"
#include "state/actor/tower.h"

namespace state {

std::vector<int64_t> Tower::max_hp_levels = {100, 200, 350};
}
