/**
 * @file soldier.h
 * Constants related to soldiers
 */

#ifndef CONSTANTS_SOLDIER_H
#define CONSTANTS_SOLDIER_H

#include "physics/vector.h"
#include <cstdint>

// Number of soldiers per player
const int64_t NUM_SOLDIERS = 20;

const int64_t SOLDIER_MAX_HP = 100;

const int64_t SOLDIER_SPEED = 50;

// IMPORTANT: Keep this higher than soldier move speed
const int64_t SOLDIER_ATTACK_RANGE = 60;

const int64_t SOLDIER_ATTACK_DAMAGE = 50;

// Number of turns a soldier takes to respawn
// Must be greater than 0, otherwise horrible things will occur
const int64_t SOLDIER_TOTAL_TURNS_TO_RESPAWN = 10;

#endif
