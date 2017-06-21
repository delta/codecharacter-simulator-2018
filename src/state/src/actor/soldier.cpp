/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "physics/vector.h"
#include "state/actor/soldier.h"

namespace state {

Soldier::Soldier() {
	// Init none
}

Soldier::Soldier(
	ActorId id,
	PlayerId player_id,
	ActorType actor_type,
	int64_t hp,
	int64_t max_hp,
	physics::Vector position,
	int64_t max_speed,
	physics::Vector velocity
	):
	Actor(
		id,
		player_id,
		actor_type,
		hp,
		max_hp,
		position
	),
	max_speed(max_speed),
	velocity(velocity) {}

physics::Vector Soldier::GetVelocity() {
	return velocity;
}

int64_t Soldier::GetMaxSpeed() {
	return max_speed;
}

void Soldier::Update() {}

}
