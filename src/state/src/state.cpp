/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {
	State::State() {
		// Init None
	}

	State::State(
		std::vector<Soldier*> p_soldiers,
		IMap* map,
		MoneyManager money_manager
		): map(map),
		money_manager(money_manager) {
		for(int i = 0; i < p_soldiers.size(); ++i) {
			soldiers.emplace_back(std::move(p_soldiers[i]));
		}
	}

	std::vector<Soldier*> State::GetAllSoldiers() {
		std::vector<Soldier*> ret_soldiers;
		for(int i = 0; i < soldiers.size(); ++i) {
			ret_soldiers.push_back(soldiers[i].get());
		}
		return ret_soldiers;
	}
}
