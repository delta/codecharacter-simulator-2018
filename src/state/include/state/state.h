/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#ifndef STATE_STATE_H
#define STATE_STATE_H

#include <iostream>
#include <vector>
#include <memory>
#include "physics/vector.h"
#include "state/actor/soldier.h"
#include "state/map/interfaces/i_map.h"
#include "state/state_export.h"

namespace state {

class STATE_EXPORT State {
protected:
	/**
	 * List of all soldiers in the game
	 */
	std::vector<std::unique_ptr<Soldier> > soldiers;

	/**
	 * Map of the game
	 */
	std::unique_ptr<IMap> map;

public:
	State();

	State& operator=(const State& other) = delete;

	State(const State& other) = delete;

	State(
		std::vector<Soldier*> soldiers,
		IMap* map
	);

	~State() = default;

	/**
	 * Get all soldiers
	 *
	 * @return      Vector of all soldiers in state
	 */
	std::vector<Soldier*> GetAllSoldiers();
};

}

#endif
