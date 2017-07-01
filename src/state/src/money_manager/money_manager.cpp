/**
 * @file money_manager.cpp
 * Defines the methods of the MoneyManager Class
 */

#include <stdexcept>
#include "physics/vector.h"
#include "state/money_manager/money_manager.h"

namespace state {

MoneyManager::MoneyManager() {
	// Init None
}

MoneyManager::MoneyManager(
	std::vector<int64_t> player_money,
	int64_t max_money
	): player_money(player_money),
	max_money(max_money) {}

void MoneyManager::Increase(
	state::PlayerId player_id,
	int64_t amount
) {

	int64_t current_player_id = static_cast<int>(player_id);

	if (amount <= 0) {
		throw std::out_of_range("`amount` must be positive");
	};

	player_money[current_player_id] += amount;

	if (player_money[current_player_id] > max_money) {
		player_money[current_player_id] = max_money;
	}
}

void MoneyManager::Decrease(
	state::PlayerId player_id,
	int64_t amount
) {

	int64_t current_player_id = static_cast<int>(player_id);

	if (amount <= 0) {
		throw std::out_of_range("`amount` must be positive");
	};

	if (amount > player_money[current_player_id]) {
		throw std::out_of_range("`amount` is greater than current balance");
	}

	player_money[current_player_id] -= amount;
}

int64_t MoneyManager::GetBalance(state::PlayerId player_id) {
	return player_money[static_cast<int>(player_id)];
}

int64_t MoneyManager::GetMaxMoney() {
	return max_money;
}

}
