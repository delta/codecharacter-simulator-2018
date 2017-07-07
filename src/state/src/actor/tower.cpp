/**
 * @file tower.cpp
 * Defines the methods of the Tower class
 */

#include "physics/vector.h"
#include "state/actor/tower.h"

namespace state {

Tower::Tower() {
	// Init none
}

Tower::Tower(
	ActorId id,
	PlayerId player_id,
	ActorType actor_type,
	int64_t hp,
	int64_t max_hp,
	physics::Vector position,
	bool is_base,
	TowerLevel tower_level
	):
	Actor(
		id,
		player_id,
		actor_type,
		hp,
		max_hp,
		position
	),
	is_base(is_base),
	tower_level(tower_level) {}

bool Tower::GetIsBase() {
	return is_base;
}

TowerLevel Tower::GetTowerLevel() {
	return tower_level;
}

void Tower::Upgrade(int64_t new_hp) {
	this->tower_level++;

	this->max_hp = new_hp;
	this->hp = new_hp;
}

void Tower::Update() {}

}
