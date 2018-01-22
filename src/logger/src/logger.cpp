/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"
#include "constants/constants.h"
#include "state/actor/tower.h"
#include "state/tower_manager/tower_manager.h"

using namespace state;

namespace logger {

Logger::Logger(int64_t player_instruction_limit_turn,
               int64_t player_instruction_limit_game)
    : turn_count(0), tower_logs(), logs(std::make_unique<proto::Game>()),
      instruction_counts(std::vector<int64_t>((int)PlayerId::PLAYER_COUNT, 0)),
      error_map(std::unordered_map<std::string, int64_t>()), current_error_code(0),
      errors(std::vector<std::vector<int64_t>>(
          (int)state::PlayerId::PLAYER_COUNT, std::vector<int64_t>())),
      player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game) {}

void Logger::LogState(IState *state) {
	turn_count++;
	auto *game_state = logs->add_states();

	auto soldiers = state->GetAllSoldiers();
	auto towers = state->GetAllTowers();
	auto money = state->GetMoney();

	if (turn_count == 1) {
		// Stuff that should only be done on the first turn
		// Set the terrain properties
		auto *map = state->GetMap();
		auto map_size = map->GetSize();
		auto element_size = map->GetElementSize();

		logs->set_terrain_size(map_size);
		logs->set_terrain_element_size(element_size);

		// Set the tower ranges
		for (auto tower_range : TowerManager::tower_ranges) {
			logs->add_tower_ranges(tower_range);
		}

		// Set the tower Max HPs
		for (auto tower_max_hp : Tower::max_hp_levels) {
			logs->add_tower_max_hps(tower_max_hp);
		}

		// Set soldier max HP
		// ALERT! - Cannot determine soldier max_hp if there are none
		if (soldiers.size() == 0 || soldiers[0].size() == 0) {
			throw std::logic_error("There are no soldiers. Cannot log.");
		}
		logs->set_soldier_max_hp(soldiers[0][0]->GetMaxHp());

		// Set towers, and add tower log entries
		for (auto player_towers : towers) {
			std::vector<TowerLogEntry> player_tower_log_entry;
			for (auto tower : player_towers) {
				proto::Tower *t_tower = game_state->add_towers();
				t_tower->set_id((int)tower->GetActorId());
				t_tower->set_player_id((int)tower->GetPlayerId());
				t_tower->set_hp(tower->GetHp());
				t_tower->set_x((int)tower->GetPosition().x);
				t_tower->set_y((int)tower->GetPosition().y);
				t_tower->set_is_base(tower->GetIsBase());
				t_tower->set_tower_level(tower->GetTowerLevel());
				t_tower->set_is_dead(false);

				player_tower_log_entry.push_back({(int)tower->GetActorId(),
				                                  tower->GetHp(),
				                                  (int)tower->GetTowerLevel()});
			}
			tower_logs.push_back(player_tower_log_entry);
		}

		// Set instruction limit constants
		logs->set_inst_limit_turn(this->player_instruction_limit_turn);
		logs->set_inst_limit_game(this->player_instruction_limit_game);

	} else {
		// Stuff that's done only on subsequent turns

		// Iterate through the players
		for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; ++i) {
			int log_ptr = 0;
			int tower_ptr = 0;

			// Iterate through a player's tower list
			while (log_ptr < tower_logs[i].size() &&
			       tower_ptr < towers[i].size()) {

				auto curr_log = tower_logs[i][log_ptr];
				auto curr_tower = towers[i][tower_ptr];

				// Check if a tower exists in both lists
				if (curr_log.id == (int)curr_tower->GetActorId()) {

					// If it does, check if it's stats have changed
					if (curr_log.hp != curr_tower->GetHp() ||
					    curr_log.level != (int)curr_tower->GetTowerLevel()) {

						// An existing tower's stats have changed. Log it
						auto *t_tower = game_state->add_towers();
						t_tower->set_id((int)curr_tower->GetActorId());
						t_tower->set_player_id(i);
						t_tower->set_hp(curr_tower->GetHp());
						t_tower->set_tower_level(curr_tower->GetTowerLevel());

						// Update the tower log as well
						tower_logs[i][log_ptr].hp = curr_tower->GetHp();
						tower_logs[i][log_ptr].level =
						    (int)curr_tower->GetTowerLevel();
					}

					log_ptr++;
					tower_ptr++;

				} else if (curr_log.id < curr_tower->GetActorId()) {
					// If a tower dies, the main tower id will be greater

					// A tower was destroyed. Log this tower from the log.
					auto *t_tower = game_state->add_towers();
					t_tower->set_id(curr_log.id);
					t_tower->set_is_dead(true);

					// Remove it from the tower log
					tower_logs[i].erase(tower_logs[i].begin() + log_ptr);

					// Don't log_ptr++, size of tower_logs already reduced

				} else {
					// The tower hasn't changed. Don't log anything.
					log_ptr++;
					tower_ptr++;
				}
			}

			// Remaining towers in the log list have been destroyed
			while (log_ptr < tower_logs[i].size()) {
				auto curr_log = tower_logs[i][log_ptr];

				// A tower was destroyed. Log it.
				auto *t_tower = game_state->add_towers();
				t_tower->set_id(curr_log.id);
				t_tower->set_is_dead(true);

				// Remove it from the tower log
				tower_logs[i].erase(tower_logs[i].begin() + log_ptr);

				// Don't log_ptr++, size of tower_logs already reduced
			}

			// Remaining towers in the main list are newly built
			while (tower_ptr < towers[i].size()) {
				auto *curr_tower = towers[i][tower_ptr];

				// A newly built tower. Log it.
				auto *t_tower = game_state->add_towers();
				t_tower->set_id((int)curr_tower->GetActorId());
				t_tower->set_player_id((int)curr_tower->GetPlayerId());
				t_tower->set_hp(curr_tower->GetHp());
				t_tower->set_x((int)curr_tower->GetPosition().x);
				t_tower->set_y((int)curr_tower->GetPosition().y);
				t_tower->set_is_base(curr_tower->GetIsBase());
				t_tower->set_tower_level(curr_tower->GetTowerLevel());
				t_tower->set_is_dead(false);

				// Add to the tower log as well
				tower_logs[i].push_back({(int)curr_tower->GetActorId(),
				                         curr_tower->GetHp(),
				                         (int)curr_tower->GetTowerLevel()});
				tower_ptr++;
			}
		}
	}

	// Stuff that's done on all turns

	// Log all the soldiers
	for (auto player_soldiers : soldiers) {
		for (auto soldier : player_soldiers) {
			auto *t_soldier = game_state->add_soldiers();
			t_soldier->set_hp(soldier->GetHp());
			t_soldier->set_x((int)soldier->GetPosition().x);
			t_soldier->set_y((int)soldier->GetPosition().y);
			switch (soldier->GetState()) {
			case SoldierStateName::ATTACK:
				t_soldier->set_state(proto::ATTACK);
				break;
			case SoldierStateName::PURSUIT:
			case SoldierStateName::MOVE:
				t_soldier->set_state(proto::MOVE);
				break;
			case SoldierStateName::IDLE:
				t_soldier->set_state(proto::IDLE);
				break;
			case SoldierStateName::DEAD:
				t_soldier->set_state(proto::DEAD);
				break;
			}
		}
	}

	// Log player money
	for (auto player_money : money) {
		game_state->add_money(player_money);
	}

	// Log instruction counts and reset temp counts to 0
	for (auto &inst_count : instruction_counts) {
		game_state->add_instruction_counts(inst_count);
		inst_count = 0;
	}

	// Log the errors, clear the error vectors
	for (auto &player_errors : errors) {
		auto player_error_struct = game_state->add_player_errors();
		for (auto error_code : player_errors) {
			player_error_struct->add_errors(error_code);
		}
		player_errors.clear();
	}
}

void Logger::LogInstructionCount(PlayerId player_id, int64_t count) {
	this->instruction_counts[(int)player_id] = count;
}

void Logger::LogError(state::PlayerId player_id, ErrorType error_type,
                      std::string message) {
	int64_t error_code;

	// Encode the message as "ERROR_TYPE: <message_string>"
	std::string full_message = ErrorTypeName[(int)error_type] + ": " + message;

	// If the message doesn't exist in the map, add an entry in the map and
	// increment the counter
	if (error_map.find(full_message) == error_map.end()) {
		error_code = current_error_code++;
		error_map[full_message] = error_code;
	} else {
		error_code = error_map[full_message];
	}

	errors[(int)player_id].push_back(error_code);
}

void Logger::LogFinalGameParams() {
	// Write the error mapping to logs
	// Flip the mapping, int error_code -> string message
	for (auto element : error_map) {
		(*logs->mutable_error_map())[element.second] = element.first;
	}
}

void Logger::WriteGame(std::ostream &write_stream) {
	logs->SerializeToOstream(&write_stream);
}
}
