#include "game.pb.h"
#include "logger/logger.h"
#include "state/mocks/map_mock.h"
#include "state/mocks/state_mock.h"
#include "gtest/gtest.h"
#include <sstream>

using namespace std;
using namespace ::testing;
using namespace state;
using namespace logger;

class LoggerTest : public testing::Test {
  protected:
	std::unique_ptr<Logger> logger;
	std::unique_ptr<StateMock> state;
	std::unique_ptr<MapMock> map;

	LoggerTest()
	    : logger(make_unique<Logger>()), state(make_unique<StateMock>()),
	      map(make_unique<MapMock>()) {}
};

TEST_F(LoggerTest, WriteReadTest) {
	Actor::SetActorIdIncrement(0);

	// Attempts to log all state entities, with towers being a focus
	// Each towersn object serves as the return object for GetAllTowers on
	// every turn. Each copies the previous tower state and modifies it to test
	// a scenario

	// Initialize one soldier for each team
	// NOTE: Init nullptr in place of pathplanner. Soldier move will segfault
	auto *soldier =
	    new Soldier(Actor::GetNextActorId(), state::PlayerId::PLAYER1,
	                state::ActorType::SOLDIER, 100, 100,
	                physics::Vector(20, 20), 5, 5, 40, nullptr);
	auto *soldier2 =
	    new Soldier(Actor::GetNextActorId(), state::PlayerId::PLAYER2,
	                state::ActorType::SOLDIER, 100, 100,
	                physics::Vector(20, 20), 5, 5, 40, nullptr);
	vector<vector<Soldier *>> soldiers;
	vector<Soldier *> player_soldiers;
	vector<Soldier *> player_soldiers2;
	player_soldiers.push_back(soldier);
	player_soldiers2.push_back(soldier2);
	soldiers.push_back(player_soldiers);
	soldiers.push_back(player_soldiers2);

	// BASE TOWERS CASE
	// Make a tower per team
	auto *tower =
	    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::TOWER,
	              500, 500, physics::Vector(20, 10), false, 1);
	auto *tower2 =
	    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER2, ActorType::TOWER,
	              500, 500, physics::Vector(5, 5), false, 1);
	vector<vector<Tower *>> towers;
	vector<Tower *> player_towers;
	vector<Tower *> player_towers2;
	player_towers.push_back(tower);
	player_towers2.push_back(tower2);
	towers.push_back(player_towers);
	towers.push_back(player_towers2);

	// TOWER BUILD CASE
	// Make a copy and add two towers (to be built on the second turn)
	auto *tower3 =
	    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::TOWER,
	              500, 500, physics::Vector(15, 15), false, 1);
	auto *tower4 =
	    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::TOWER,
	              500, 500, physics::Vector(3, 25), false, 1);
	vector<vector<Tower *>> towers2 = towers;
	towers2[0].push_back(tower3);
	towers2[0].push_back(tower4);

	// TOWER DESTROY CASE
	// Make another copy and remove the middle tower
	// tower and tower4 remain
	vector<vector<Tower *>> towers3 = towers2;
	towers3[0].erase(towers3[0].begin() + 1);

	// TOWER CHANGE CASE
	// Make another copy and alter a tower's HP (done later)
	vector<vector<Tower *>> towers4 = towers3;

	// SIMULTANEOUS TOWERS BUILD AND DESTROY CASE
	// Make another copy, destroy both player1 towers and add a new tower
	auto *tower5 =
	    new Tower(Actor::GetNextActorId(), PlayerId::PLAYER1, ActorType::TOWER,
	              500, 500, physics::Vector(25, 3), false, 1);
	vector<vector<Tower *>> towers5 = towers4;
	towers5[0].erase(towers5[0].begin(), towers5[0].begin() + 2);
	towers5[0].push_back(tower5);

	// Dummy Map element
	auto *map_element_land = new MapElement(Vector(0, 0), TerrainType::LAND);

	// Dummy money values
	vector<int64_t> money1 = {400, 500};
	vector<int64_t> money2 = {300, 600};

	EXPECT_CALL(*state, GetMap()).WillOnce(Return(map.get()));

	EXPECT_CALL(*map, GetSize()).WillOnce(Return(30));

	EXPECT_CALL(*map, GetElementByOffset(_))
	    .Times(30 * 30)
	    .WillRepeatedly(ReturnRef(*map_element_land));

	EXPECT_CALL(*state, GetMoney())
	    .WillOnce(Return(money1))
	    .WillRepeatedly(Return(money2));

	EXPECT_CALL(*state, GetAllSoldiers()).WillRepeatedly(Return(soldiers));

	EXPECT_CALL(*state, GetAllTowers())
	    .WillOnce(Return(towers))
	    .WillOnce(Return(towers2))
	    .WillOnce(Return(towers3))
	    .WillOnce(Return(towers4))
	    .WillRepeatedly(Return(towers5));

	// Run 3 turns, update HP, run the remaining turns
	logger->LogState(state.get());
	logger->LogState(state.get());
	logger->LogState(state.get());
	tower2->SetHp(1);
	logger->LogState(state.get());
	logger->LogState(state.get());

	ostringstream str_stream;
	logger->WriteGame(str_stream);
	string result_string = str_stream.str();

	auto game = make_unique<proto::Game>();
	game->ParseFromString(result_string);

	// Check if money is fine
	ASSERT_EQ(game->states(0).money(0), money1[0]);
	ASSERT_EQ(game->states(0).money(1), money1[1]);
	ASSERT_EQ(game->states(1).money(0), money2[0]);
	ASSERT_EQ(game->states(1).money(1), money2[1]);

	// Check for terrain
	ASSERT_EQ(game->terrain().rows_size(), 30);
	ASSERT_EQ(game->terrain().rows(0).elements_size(), 30);
	ASSERT_EQ(game->terrain().rows(0).elements(0).type(),
	          proto::TerrainElement::LAND);

	// Check if soldiers are there
	ASSERT_EQ(game->states(0).soldiers_size(), 2);
	ASSERT_EQ(game->states(1).soldiers_size(), 2);

	// BASE TOWERS CASE
	// Check if both towers are there in the first turn
	ASSERT_EQ(game->states(0).towers(0).id(), tower->GetActorId());
	ASSERT_EQ(game->states(0).towers(1).id(), tower2->GetActorId());

	// TOWER BUILD CASE
	// Ensure only the newly built towers are there on the second turn
	ASSERT_EQ(game->states(1).towers_size(), 2);
	ASSERT_EQ(game->states(1).towers(0).id(), tower3->GetActorId());
	ASSERT_EQ(game->states(1).towers(1).id(), tower4->GetActorId());

	// TOWER DESTROY CASE
	// Ensure only the destroyed tower is there on the third turn
	ASSERT_EQ(game->states(2).towers_size(), 1);
	ASSERT_EQ(game->states(2).towers(0).id(), tower3->GetActorId());
	ASSERT_TRUE(game->states(2).towers(0).is_dead());

	// TOWER CHANGE CASE
	// Ensure only the altered tower is there on the fourth turn
	ASSERT_EQ(game->states(3).towers_size(), 1);
	ASSERT_EQ(game->states(3).towers(0).id(), tower2->GetActorId());
	ASSERT_EQ(game->states(3).towers(0).hp(), 1);

	// SIMULTANEOUS TOWERS BUILD AND DESTROY CASE
	// Ensure both dead towers and new tower on the fifth turn
	ASSERT_EQ(game->states(4).towers_size(), 3);
	ASSERT_EQ(game->states(4).towers(0).id(), tower->GetActorId());
	ASSERT_TRUE(game->states(2).towers(0).is_dead());
	ASSERT_EQ(game->states(4).towers(1).id(), tower4->GetActorId());
	ASSERT_TRUE(game->states(2).towers(0).is_dead());
	ASSERT_EQ(game->states(4).towers(2).id(), tower5->GetActorId());
}
