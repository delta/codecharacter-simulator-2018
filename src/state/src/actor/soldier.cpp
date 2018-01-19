/**
 * @file soldier.cpp
 * Defines the methods of the Soldier class
 */

#include "state/actor/soldier.h"
#include "physics/vector.h"
#include "state/actor/soldier_states/idle_state.h"
#include "state/actor/soldier_states/soldier_state.h"

namespace state {

Soldier::Soldier() {
	// Init none
}
Soldier::Soldier(ActorId id, PlayerId player_id, ActorType actor_type,
                 int64_t hp, int64_t max_hp, physics::Vector position,
                 int64_t speed, int64_t attack_range, int64_t attack_damage,
                 IPathPlanner *path_planner, MoneyManager *money_manager)
    : Actor(id, player_id, actor_type, hp, max_hp, position), speed(speed),
      attack_range(attack_range), attack_damage(attack_damage),
      path_planner(path_planner), money_manager(money_manager),
      attack_target(nullptr), destination(physics::Vector(0, 0)),
      is_destination_set(false), new_position(physics::Vector(0, 0)),
      is_new_position_set(false), damage_incurred(0),
      state(std::make_unique<IdleState>(this)) {}

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

void Soldier::ClearDestination() { this->is_destination_set = false; }

bool Soldier::IsDestinationSet() { return is_destination_set; }

physics::Vector Soldier::GetNewPosition() { return new_position; }

void Soldier::SetNewPosition(physics::Vector new_position) {
	this->new_position = new_position;
	this->is_new_position_set = true;
}

void Soldier::ClearNewPosition() { this->is_new_position_set = false; }

bool Soldier::IsNewPositionSet() { return is_new_position_set; }

bool Soldier::IsAttackTargetSet() {
	return attack_target == nullptr ? false : true;
}

void Soldier::SetPosition(physics::Vector position) {
	this->position = position;
}

IPathPlanner *Soldier::GetPathPlanner() { return path_planner; }

MoneyManager *Soldier::GetMoneyManager() { return money_manager; }

SoldierStateName Soldier::GetState() { return state->GetName(); }

void Soldier::Move(physics::Vector destination) {
	this->destination = destination;
	this->is_destination_set = true;
	this->attack_target = nullptr;
}

void Soldier::Attack(Actor *attack_target) {
	this->attack_target = attack_target;
	this->is_destination_set = false;
}

int64_t Soldier::GetLatestHp() { return hp - damage_incurred; }

void Soldier::Damage(int64_t damage_amount) {
	this->damage_incurred =
	    std::min<int64_t>(this->hp, this->damage_incurred + damage_amount);
}

void Soldier::LateUpdate() {
	// If a move was performed, copy new_position into position
	if (IsNewPositionSet()) {
		SetPosition(new_position);
		ClearNewPosition();
	}

	// Update HP and reset damage counter
	this->SetHp(this->GetLatestHp());
	this->damage_incurred = 0;

	// Allow soldier to transition to dead state if it's dead
	if (this->hp == 0 && state->GetName() != SoldierStateName::DEAD) {
		auto new_state = std::move(state->Update());
		state->Exit();
		state = std::move(new_state);
		state->Enter();
		state->Update();
	}
}

void Soldier::Update() {
	auto new_state = std::move(state->Update());

	while (new_state != nullptr) {
		// State transition has occured
		state->Exit();
		state = std::move(new_state);
		state->Enter();
		new_state = std::move(state->Update());
	}
}
}
