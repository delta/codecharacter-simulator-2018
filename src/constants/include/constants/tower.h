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
    physics::Vector(
        (MAP_SIZE / 6 - 1) * MAP_ELEMENT_SIZE + MAP_ELEMENT_SIZE / 2,
        (MAP_SIZE / 6 - 1) * MAP_ELEMENT_SIZE + MAP_ELEMENT_SIZE / 2),
    physics::Vector(
        (MAP_SIZE * 5 / 6) * MAP_ELEMENT_SIZE + MAP_ELEMENT_SIZE / 2,
        (MAP_SIZE * 5 / 6) * MAP_ELEMENT_SIZE + MAP_ELEMENT_SIZE / 2)};

const auto TOWER_HPS = std::vector<int64_t>{7000, 10000, 15000};

const auto TOWER_BUILD_COSTS = std::vector<int64_t>{500, 300, 300};

const auto TOWER_RANGES = std::vector<int64_t>{2, 3, 4};

const int64_t MAX_NUM_TOWERS = 15;

#endif
