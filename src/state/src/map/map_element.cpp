/**
 * @file map_element.cpp
 * Definitions for map elements
 */

#include "state/map/map_element.h"

namespace state {

MapElement::MapElement(
	physics::Vector position,
	TerrainType terrain_type) :
	position(position),
	terrain_type(terrain_type),
	ownership(static_cast<int>(PlayerId::PLAYER_COUNT), false) {}

physics::Vector MapElement::GetPosition() {
	return this->position;
}

TerrainType MapElement::GetTerrainType() {
	return this->terrain_type;
}

std::vector<bool> MapElement::GetOwnership() {
	return this->ownership;
}

void MapElement::SetOwnership(PlayerId player_id, bool ownership) {
	this->ownership[static_cast<int>(player_id)] = ownership;
}

}
