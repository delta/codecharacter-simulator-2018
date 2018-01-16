/**
 * @file tower.h
 * Constants related to towers
 */

#ifndef CONSTANTS_TOWER_H
#define CONSTANTS_TOWER_H

#include "map.h"
#include "physics/vector.h"
#include <vector>

// Positions for the base towers indexed by player ID
const auto BASE_TOWER_POSITIONS = std::vector<physics::Vector>{
    physics::Vector(MAP_SIZE * MAP_ELEMENT_SIZE / 6,
                    MAP_SIZE *MAP_ELEMENT_SIZE / 6),
    physics::Vector(MAP_SIZE *MAP_ELEMENT_SIZE * 5 / 6,
                    MAP_SIZE *MAP_ELEMENT_SIZE * 5 / 6)};

const auto TOWER_HPS = std::vector<int64_t>{100, 200, 350};

const auto TOWER_BUILD_COSTS = std::vector<int64_t>{500, 700, 900};

const auto TOWER_RANGES = std::vector<int64_t>{1, 2, 3};

const int64_t MAX_NUM_TOWERS = 40;

#endif
