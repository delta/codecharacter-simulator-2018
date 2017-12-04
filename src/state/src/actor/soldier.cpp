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
                 int64_t speed, SoldierState soldier_state,
                 int64_t attack_range, int64_t attack_damage)
    : Actor(id, player_id, actor_type, hp, max_hp, position), speed(speed),
      respawn_system(std::make_unique<RespawnSystem>(this)),
      soldier_state(soldier_state), attack_range(attack_range),
      attack_damage(attack_damage) {}

int64_t Soldier::GetSpeed() { return speed; }

SoldierState Soldier::GetState() { return soldier_state; }

int64_t Soldier::GetAttackRange() { return attack_range; }

int64_t Soldier::GetAttackDamage() { return attack_damage; }

void Soldier::SetState(SoldierState p_soldier_state) {
	this->soldier_state = p_soldier_state;
}

void Soldier::SetPosition(physics::Vector position) {
	this->position = position;
}

void Soldier::Update() {
	this->respawn_system->Update();
	if (this->respawn_system->IsReadyToRespawn()) {
		this->hp = this->max_hp;
		this->position = RespawnSystem::respawn_position;
		this->SetState(SoldierState::IDLE);
	}
}
}
