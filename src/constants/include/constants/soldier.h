/**
 * @file soldier.h
 * Constants related to soldiers
 */

#ifndef CONSTANTS_SOLDIER_H
#define CONSTANTS_SOLDIER_H

#include "physics/vector.h"
#include <cstdint>

// Number of soldiers per player
const int64_t NUM_SOLDIERS = 10;

const int64_t SOLDIER_MAX_HP = 100;

const int64_t SOLDIER_SPEED = 50;

const int64_t SOLDIER_ATTACK_RANGE = 30;

const int64_t SOLDIER_ATTACK_DAMAGE = 25;

// Number of turns a soldier takes to respawn
const int64_t SOLDIER_TOTAL_TURNS_TO_RESPAWN = 10;

#endif
