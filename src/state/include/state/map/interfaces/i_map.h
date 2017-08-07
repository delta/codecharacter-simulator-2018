/**
 * @file i_map.h
 * Declaration for an interface to a map
 */

#ifndef STATE_MAP_INTERFACES_I_MAP_H
#define STATE_MAP_INTERFACES_I_MAP_H

#include "physics/vector.h"
#include "state/map/map_element.h"
#include "state/state_export.h"

namespace state {

class STATE_EXPORT IMap {
public:

	virtual ~IMap() {}

	/**
	 * Gets an element by x, y co-ordinates
	 *
	 * @param[in]  position  The x, y co-ordinates
	 *
	 * @return     The element by xy
	 */
	virtual MapElement& GetElementByXY(physics::Vector position) = 0;

	/**
	 * Gets an element by its offset in the 2D grid of elements
	 *
	 * @param[in]  position  The offset
	 *
	 * @return     The element by offset
	 */
	virtual MapElement& GetElementByOffset(physics::Vector position) = 0;

	/**
	 * Gets the size of the map (width/height) in offsets
	 *
	 * @return     The map's size
	 */
	virtual int64_t GetSize() = 0;

	/**
	 * Gets the size of an element of the map
	 *
	 * @return     The element size
	 */
	virtual int64_t GetElementSize() = 0;
};

}

#endif
