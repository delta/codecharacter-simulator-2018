/**
 * @file money_manager.h
 * Constants related to money
 */

#ifndef CONSTANTS_MONEY_MANAGER_H
#define CONSTANTS_MONEY_MANAGER_H

#include <cstdint>
#include <vector>

// Amount of money each player has at the start of the game
const int64_t MONEY_START = 300;

// Maxiumm amount of money each player can have
const int64_t MONEY_MAX = INT64_MAX;

// Rewards for killing towers indexed by tower level
const auto TOWER_KILL_REWARD_AMOUNTS = std::vector<int64_t>{60, 120, 180};

// Reward for killing one soldier
const int64_t SOLDIER_KILL_REWARD_AMOUNT = 20;

// Rewards for tower sepukku indexed by tower level
const auto TOWER_SUICIDE_REWARD_AMOUNT = std::vector<int64_t>{20, 40, 60};

#endif
