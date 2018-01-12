/**
 * @file money_manager.cpp
 * Defines the methods of the MoneyManager Class
 */

#include "state/money_manager/money_manager.h"
#include "physics/vector.h"
#include <stdexcept>

namespace state {

MoneyManager::MoneyManager() {
	// Init None
}

MoneyManager::MoneyManager(std::vector<int64_t> player_money, int64_t max_money,
                           std::vector<int64_t> tower_kill_reward_amount,
                           int64_t soldier_kill_reward_amount,
                           std::vector<int64_t> tower_suicide_reward_amount)
    : player_money(player_money), max_money(max_money),
      tower_kill_reward_amount(tower_kill_reward_amount),
      soldier_kill_reward_amount(soldier_kill_reward_amount),
      tower_suicide_reward_amount(tower_suicide_reward_amount) {}

void MoneyManager::Increase(state::PlayerId player_id, int64_t amount) {

	int64_t current_player_id = static_cast<int>(player_id);

	if (amount <= 0) {
		throw std::out_of_range("`amount` must be positive");
	};

	player_money[current_player_id] += amount;

	if (player_money[current_player_id] > max_money) {
		player_money[current_player_id] = max_money;
	}
}

void MoneyManager::Decrease(state::PlayerId player_id, int64_t amount) {

	int64_t current_player_id = static_cast<int>(player_id);

	if (amount <= 0) {
		throw std::out_of_range("`amount` must be positive");
	};

	if (amount > player_money[current_player_id]) {
		throw std::out_of_range("`amount` is greater than current balance");
	}

	player_money[current_player_id] -= amount;
}

void MoneyManager::RewardKill(Actor *enemy_actor) {
	PlayerId player_id;
	PlayerId enemy_player_id = enemy_actor->GetPlayerId();

	if (enemy_player_id == PlayerId::PLAYER2) {
		player_id = PlayerId::PLAYER1;
	} else {
		player_id = PlayerId::PLAYER2;
	}

	ActorType enemy_actor_type;
	int64_t reward_amount;

	if (enemy_actor->GetActorType() == ActorType::SOLDIER) {
		reward_amount = soldier_kill_reward_amount;

	} else if (enemy_actor->GetActorType() == ActorType::TOWER) {
		auto tower_level = static_cast<Tower *>(enemy_actor)->GetTowerLevel();
		reward_amount = tower_kill_reward_amount[tower_level - 1];
	}

	Increase(player_id, reward_amount);
}

int64_t MoneyManager::GetBalance(state::PlayerId player_id) {
	return player_money[static_cast<int>(player_id)];
}

void MoneyManager::RewardSuicide(Tower *tower) {
	auto tower_level = tower->GetTowerLevel();
	auto player_id = tower->GetPlayerId();

	Increase(player_id, tower_suicide_reward_amount[tower_level - 1]);
};

int64_t MoneyManager::GetMaxMoney() { return max_money; }
}
