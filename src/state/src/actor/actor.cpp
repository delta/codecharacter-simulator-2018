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

ActorId Actor::actor_id_increment = 0;

ActorId Actor::GetActorId() {
	return id;
}

void Actor::SetActorIdIncrement(ActorId actor_id) {
	if (actor_id < 0) {
		throw std::out_of_range("`actor_id` cannot be negative");
	}
	actor_id_increment = actor_id;
}

ActorId Actor::GetNextActorId() {
	return actor_id_increment++;
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

void Actor::SetHp(int64_t hp) {
	if (hp < 0) {
		throw std::out_of_range("`hp` must be a positive value");
	}
	if (hp > this->max_hp) {
		throw std::out_of_range("`hp` cannot be greater than max_hp");
	}
	this->hp = hp;
}

physics::Vector Actor::GetPosition() {
	return position;
}

}
