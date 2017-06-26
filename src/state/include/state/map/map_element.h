/**
 * @file map_element.h
 * Declaration for a block in the map
 */

#ifndef STATE_MAP_MAP_ELEMENT_H
#define STATE_MAP_MAP_ELEMENT_H

#include <vector>
#include "physics/vector.h"
#include "state/map/terrain_type.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {

class STATE_EXPORT MapElement {
private:

	/**
	 * X, Y co-ordinates of this element
	 */
	physics::Vector position;

	/**
	 * Terrain type of this element
	 */
	TerrainType terrain_type;

	/**
	 * Ownership status of this element
	 *
	 * It's a PlayerId to bool map, true if the player owns it,
	 * false otherwise
	 */
	std::vector<bool> ownership;

public:

	MapElement(
		physics::Vector position,
		TerrainType terrain_type);

	/**
	 * Gets the element's x, y co-ordinates
	 *
	 * @return     The co-ordinates
	 */
	physics::Vector GetPosition();

	/**
	 * Gets the element's terrain type
	 *
	 * @return     The terrain type
	 */
	TerrainType GetTerrainType();

	/**
	 * Gets the ownership status of this element
	 *
	 * @return     The ownership status
	 */
	std::vector<bool> GetOwnership();

	/**
	 * Sets the ownership status of this element
	 *
	 * @param[in]  player_id  ID of player whose ownership is to be set
	 * @param[in]  ownership  The ownership status
	 */
	void SetOwnership(PlayerId player_id, bool ownership);
};

}

#endif
