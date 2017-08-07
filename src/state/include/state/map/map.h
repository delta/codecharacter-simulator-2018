/**
 * @file map.h
 * Declaration for the map of the game
 */

#ifndef STATE_MAP_MAP_H
#define STATE_MAP_MAP_H

#include "physics/vector.h"
#include "state/map/interfaces/i_map.h"
#include "state/map/map_element.h"
#include "state/state_export.h"

namespace state {

class STATE_EXPORT Map : public IMap {
private:

	/**
	 * 2D grid of elements of the map
	 */
	std::vector<std::vector<MapElement> > map_elements;

	/**
	 * Size of one map_element (width/height)
	 */
	int64_t element_size;

public:

	Map() {};

	Map(
		std::vector<std::vector<MapElement> > &map_elements,
		int64_t element_size);

	/**
	 * Gets an element by x, y co-ordinates
	 *
	 * @param[in]  position  The x, y co-ordinates
	 *
	 * @return     The element by xy
	 *
	 * @throw      std::out_of_range If position does not
	 *                               point to a valid element
	 */
	MapElement& GetElementByXY(physics::Vector position) override;

	/**
	 * Gets an element by its offset in the 2D grid of elements
	 *
	 * @param[in]  position  The offset
	 *
	 * @return     The element by offset
	 *
	 * @throw      std::out_of_range If position does not
	 *                               point to a valid element
	 */
	MapElement& GetElementByOffset(physics::Vector position) override;

	/**
	 * Gets the size of the map (width/height) in offsets
	 *
	 * @return     The map's size
	 */
	int64_t GetSize() override;

	/**
	 * Gets the size of an element of the map
	 *
	 * @return     The element size
	 */
	int64_t GetElementSize() override;
};

}

#endif
