/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "state/actor/soldier.h"
#include "physics/vector.h"

namespace state {

Soldier::Soldier() {
	// Init none
}
Soldier::Soldier(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, physics::Vector position,
                 int64_t speed, int64_t attack_range, int64_t attack_damage)
    : Actor(id, player_id, actor_type, hp, max_hp, position), speed(speed),
      attack_range(attack_range), attack_damage(attack_damage),
      attack_target(nullptr), destination(physics::Vector(0, 0)),
      is_destination_set(false) {}

int64_t Soldier::GetSpeed() { return speed; }

int64_t Soldier::GetAttackRange() { return attack_range; }

int64_t Soldier::GetAttackDamage() { return attack_damage; }

Actor *Soldier::GetAttackTarget() { return attack_target; }

void Soldier::SetAttackTarget(Actor *attack_target) {
	this->attack_target = attack_target;
};

bool Soldier::IsAttackTargetInRange() {
	if (attack_target == nullptr) {
		throw std::logic_error("Attack target is not set");
	}
	auto target_position = attack_target->GetPosition();

	// Return true if the distance between the soldier and the target is
	// lesser than the attack_range
	return position.distance(target_position) <= attack_range;
}

physics::Vector Soldier::GetDestination() { return destination; }

void Soldier::SetDestination(physics::Vector destination) {
	this->destination = destination;
	this->is_destination_set = true;
}

void Soldier::ClearDestination() {
	this->is_destination_set = false;
}

bool Soldier::IsDestinationSet() { return is_destination_set; }

bool Soldier::IsAttackTargetSet() {
	return attack_target == nullptr ? false : true;
}

void Soldier::SetPosition(physics::Vector position) {
	this->position = position;
}

void Soldier::Update() {}
}
