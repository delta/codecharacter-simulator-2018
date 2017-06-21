/**
 * @file actor.cpp
 * Defines the methods of the Actor class
 */

#include <memory>
#include "physics/vector.h"
#include "state/actor/actor.h"

namespace state {

Actor::Actor() {
	// Init None
}

Actor::Actor(
	ActorId id,
	PlayerId player_id,
	ActorType actor_type,
	int64_t hp,
	int64_t max_hp,
	physics::Vector position
	):
	id(id),
	player_id(player_id),
	actor_type(actor_type),
	hp(hp),
	max_hp(max_hp),
	position(position) {}

Actor::~Actor() {}

ActorId Actor::GetActorId() {
	return id;
}

PlayerId Actor::GetPlayerId() {
	return player_id;
}

ActorType Actor::GetActorType() {
	return actor_type;
}

int64_t Actor::GetHp() {
	return hp;
}

int64_t Actor::GetMaxHp() {
	return max_hp;
}

physics::Vector Actor::GetPosition() {
	return position;
}

}
