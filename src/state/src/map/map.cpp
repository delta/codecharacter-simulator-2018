/**
 * @file map.cpp
 * Definitions for the map in the game
 */

#include <stdexcept>
#include <cmath>
#include "state/map/map.h"

namespace state {

Map::Map(
	std::vector<std::vector<MapElement> > & map_elements,
	int64_t element_size) :
	map_elements(map_elements), element_size(element_size) {}

MapElement& Map::GetElementByXY(physics::Vector position) {
	auto elt_size = this->GetElementSize();
	auto map_size = this->map_elements.size();

	if (map_size <= position.x / elt_size ||
		map_size <= position.y / elt_size ||
		position.x < 0 || position.y < 0) {
		throw std::out_of_range("`position` out of bounds");
	}

	return this->map_elements[floor(position.x / elt_size)][floor(position.y / elt_size)];
}

MapElement& Map::GetElementByOffset(physics::Vector position) {
	auto map_size = this->map_elements.size();

	if (map_size <= position.x ||
		map_size <= position.y ||
		position.x < 0 || position.y < 0) {
		throw std::out_of_range("`position` out of bounds");
	}

	return this->map_elements[floor(position.x)][floor(position.y)];
}

int64_t Map::GetSize() {
	return this->map_elements.size();
}

int64_t Map::GetElementSize() {
	return this->element_size;
}

}
