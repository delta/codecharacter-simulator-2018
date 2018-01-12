#include "state/money_manager/money_manager.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace std;
using namespace state;

class MoneyManagerTest : public testing::Test {
  protected:
	MoneyManager money_manager;

	vector<int64_t> player_money;
	int64_t soldier_kill_reward_amount;
	vector<int64_t> tower_kill_reward_amount;
	vector<int64_t> tower_suicide_reward_amount;
	int64_t max_money;

	MoneyManagerTest() {
		this->max_money = 10000;

		int64_t player_count = static_cast<int64_t>(PlayerId::PLAYER_COUNT);
		for (int i = 0; i < player_count; ++i) {
			player_money.push_back(5000); // Start balance
		}

		soldier_kill_reward_amount = 100;
		tower_kill_reward_amount = {100, 300, 1000};
		tower_suicide_reward_amount = {200, 250, 300};

		this->money_manager = MoneyManager(
		    player_money, max_money, tower_kill_reward_amount,
		    soldier_kill_reward_amount, tower_suicide_reward_amount);
	}
};

TEST_F(MoneyManagerTest, GetBalanceTest) {
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER1), 5000);
}

TEST_F(MoneyManagerTest, ValidIncreaseTest) {
	// Test For different players
	money_manager.Increase((PlayerId::PLAYER1), 2000);
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER1), 5000 + 2000);

	money_manager.Increase((PlayerId::PLAYER2), 4000);
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER2), 5000 + 4000);

	// Test for max balance
	money_manager.Increase((PlayerId::PLAYER1), max_money + 5000);
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER1), max_money);
}

TEST_F(MoneyManagerTest, InvalidIncreaseTest) {
	// Test for zero increase
	EXPECT_THROW(money_manager.Increase((PlayerId::PLAYER1), 0),
	             std::out_of_range);

	EXPECT_THROW(money_manager.Increase((PlayerId::PLAYER1), -5000),
	             std::out_of_range);
}

TEST_F(MoneyManagerTest, ValidDecreaseTest) {
	// Test For different players
	money_manager.Decrease((PlayerId::PLAYER1), 2000);
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER1), 5000 - 2000);

	money_manager.Decrease((PlayerId::PLAYER2), 4000);
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER2), 5000 - 4000);
}

TEST_F(MoneyManagerTest, InvalidDecreaseTest) {
	// Test for zero increase
	EXPECT_THROW(money_manager.Decrease((PlayerId::PLAYER1), 0),
	             std::out_of_range);

	// Test for Negative Amount
	EXPECT_THROW(money_manager.Decrease((PlayerId::PLAYER1), -5000),
	             std::out_of_range);

	// Test for Insufficient Balance
	EXPECT_THROW(money_manager.Decrease(PlayerId::PLAYER1, 20000),
	             std::out_of_range);
	ASSERT_EQ(money_manager.GetBalance(PlayerId::PLAYER1), 5000);
}
