/**
 * @file driver.h
 * Constants related to the overall game
 */

#ifndef CONSTANTS_DRIVER_H
#define CONSTANTS_DRIVER_H

#include <cmath>
#include <cstdint>

// Exceeding this many instructions in one turn results in the player forfeiting
// the turn
const int64_t PLAYER_INSTRUCTION_LIMIT_TURN = pow(10, 7) * 1;

// Exceeding this many instructions in one turn results in the player forfeiting
// the entire match
const int64_t PLAYER_INSTRUCTION_LIMIT_GAME = PLAYER_INSTRUCTION_LIMIT_TURN * 3;

// Number of turns in the game
const int64_t NUM_TURNS = 1000;

// Duration of the game in milliseconds
const int64_t GAME_DURATION_MS = 50 * 1000;

#endif
