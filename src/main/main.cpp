#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/timer.h"
#include "logger/logger.h"
#include "physics/vector.h"
#include "state/actor/actor.h"
#include "state/actor/soldier.h"
#include "state/map/interfaces/i_map.h"
#include "state/map/map.h"
#include "state/path_planner/simple_path_planner.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/state_syncer/state_syncer.h"
#include "state/utilities.h"
#include <iostream>
#include <memory>
#include <random>
#include <string>

using namespace drivers;
using namespace physics;
using namespace state;
using namespace logger;

int num_players = (int)PlayerId::PLAYER_COUNT;

auto Soldier::total_turns_to_respawn = SOLDIER_TOTAL_TURNS_TO_RESPAWN;
auto Soldier::respawn_positions = BASE_TOWER_POSITIONS;

auto Tower::max_hp_levels = TOWER_HPS;

auto TowerManager::build_costs = TOWER_BUILD_COSTS;
auto TowerManager::tower_ranges = TOWER_RANGES;

const std::string GAME_LOG_FILE_NAME = "game.log";

std::vector<std::string> shm_names(num_players);

std::string GenerateRandomString(const std::string::size_type length) {
	static std::string chars = "0123456789"
	                           "abcdefghijklmnopqrstuvwxyz"
	                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	static std::mt19937 gen{std::random_device{}()};
	static std::uniform_int_distribution<std::string::size_type> dis(
	    0, chars.length() - 1);

	std::string s(length, '*');

	for (auto &c : s)
		c = chars[dis(gen)];

	return s;
}

std::unique_ptr<Map> BuildMap() {
	std::vector<std::vector<MapElement>> map_elements;

	for (int i = 0; i < MAP_SIZE; ++i) {
		std::vector<MapElement> row;
		for (int j = 0; j < MAP_SIZE; ++j) {
			row.push_back(
			    MapElement(Vector(i * MAP_ELEMENT_SIZE, j * MAP_ELEMENT_SIZE),
			               TerrainType::LAND));
		}
		map_elements.push_back(row);
	}

	return std::make_unique<Map>(map_elements, MAP_ELEMENT_SIZE);
}

std::unique_ptr<MoneyManager> BuildMoneyManager() {
	return std::make_unique<MoneyManager>(
	    std::vector<int64_t>(num_players, MONEY_START), MONEY_MAX,
	    TOWER_KILL_REWARD_AMOUNTS, SOLDIER_KILL_REWARD_AMOUNT,
	    TOWER_SUICIDE_REWARD_AMOUNT);
}

std::unique_ptr<Soldier> BuildSoldier(PlayerId player_id,
                                      IPathPlanner *path_planner,
                                      MoneyManager *money_manager) {
	return std::make_unique<Soldier>(
	    Actor::GetNextActorId(), player_id, ActorType::SOLDIER, SOLDIER_MAX_HP,
	    SOLDIER_MAX_HP, BASE_TOWER_POSITIONS[(int)player_id], SOLDIER_SPEED,
	    SOLDIER_ATTACK_RANGE, SOLDIER_ATTACK_DAMAGE, path_planner,
	    money_manager);
}

std::unique_ptr<TowerManager>
BuildTowerManager(PlayerId player_id, MoneyManager *money_manager, IMap *map) {
	auto tower = std::make_unique<Tower>(
	    Actor::GetNextActorId(), player_id, ActorType::TOWER, TOWER_HPS[0],
	    TOWER_HPS[0], BASE_TOWER_POSITIONS[(int)player_id], true, 1);

	std::vector<std::unique_ptr<Tower>> towers;
	towers.push_back(std::move(tower));

	return std::make_unique<TowerManager>(std::move(towers), player_id,
	                                      money_manager, map);
}

std::unique_ptr<State> BuildState() {
	Actor::SetActorIdIncrement();

	auto map = std::move(BuildMap());
	auto path_planner = std::make_unique<SimplePathPlanner>(map.get());
	auto money_manager = std::move(BuildMoneyManager());

	std::vector<std::unique_ptr<TowerManager>> tower_managers(num_players);
	std::vector<std::vector<std::unique_ptr<Soldier>>> soldiers(num_players);

	for (int player_id = 0; player_id < num_players; ++player_id) {
		for (int i = 0; i < NUM_SOLDIERS; ++i) {
			soldiers[player_id].push_back(std::move(
			    BuildSoldier(static_cast<PlayerId>(player_id),
			                 path_planner.get(), money_manager.get())));
		}
	}

	for (int player_id = 0; player_id < num_players; ++player_id) {
		tower_managers[player_id] = std::move(BuildTowerManager(
		    static_cast<PlayerId>(player_id), money_manager.get(), map.get()));
	}

	return std::make_unique<State>(
	    std::move(soldiers), std::move(map), std::move(money_manager),
	    std::move(tower_managers), std::move(path_planner));
}

std::unique_ptr<drivers::MainDriver> BuildMainDriver() {
	auto logger = std::make_unique<Logger>(PLAYER_INSTRUCTION_LIMIT_TURN,
	                                       PLAYER_INSTRUCTION_LIMIT_GAME);

	auto state_syncer =
	    std::make_unique<StateSyncer>(std::move(BuildState()), logger.get(),
	                                  TOWER_BUILD_COSTS, MAX_NUM_TOWERS);
	std::vector<std::unique_ptr<SharedMemoryMain>> shm_mains;

	for (int i = 0; i < num_players; ++i) {
		shm_names[i] = GenerateRandomString(64) + std::to_string(i);
		shm_mains.push_back(std::make_unique<SharedMemoryMain>(
		    shm_names[i], false, 0, PlayerState()));
	}

	return std::make_unique<MainDriver>(
	    std::move(state_syncer), std::move(shm_mains),
	    PLAYER_INSTRUCTION_LIMIT_TURN, PLAYER_INSTRUCTION_LIMIT_GAME, NUM_TURNS,
	    num_players, Timer::Interval(GAME_DURATION_MS), std::move(logger),
	    GAME_LOG_FILE_NAME);
}

int main(int argc, char *argv[]) {
	std::string prefix_key;
	if (argc < 2) {
		prefix_key = "codecharacter";
		std::cerr
		    << "WARNING: main needs a key to prefix scores with for security,"
		       "running with default key value now...";
	} else {
		prefix_key = std::string(argv[1]);
	}

	std::cout << "Starting main...\n";
	auto driver = std::move(BuildMainDriver());

	for (int i = 1; i <= num_players; ++i) {
		std::string command =
		    "./player_" + std::to_string(i) + " " + shm_names[i - 1] + " &";
		std::system(command.c_str());
	}
	auto results = driver->Start();
	std::cout << prefix_key << " " << results[0].score << " "
	          << results[0].status << " " << results[1].score << " "
	          << results[1].status << std::endl;
	return 0;
}
