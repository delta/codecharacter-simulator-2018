/**
 * @file mobility_system.cpp
 * Defines the Attack System for soldier attacks
 */

#include "state/actor/mobility_system/mobility_system.h"
#include "state/actor/soldier.h"
#include <cmath>
#include <exception>

namespace state {

MobilitySystem::MobilitySystem(Actor *attack_target, bool attack_target_set,
                               physics::Vector destination,
                               bool destination_set,
                               bool destination_set_in_prev_turn,
                               Soldier *soldier, IMap *map)
    : attack_target(attack_target), attack_target_set(attack_target_set),
      destination(destination), destination_set(destination_set),
      destination_set_in_prev_turn(destination_set_in_prev_turn),
      soldier(soldier), map(map) {}

bool MobilitySystem::IsAttackTargetInRange() {
	auto target_position = this->attack_target->GetPosition();
	auto current_position = this->soldier->GetPosition();
	auto attack_range = this->soldier->GetAttackRange();

	// Return if the distance between the soldier and the target is
	// lesser than the attack_range
	return current_position.distance(target_position) < attack_range;
}

void MobilitySystem::AttackActor(Actor *p_attack_target) {
	// TODO: Check if the attack actor is a valid actor

	// Ensure that the attack target isn't the soldier itself
	if (p_attack_target->GetActorId() == this->soldier->GetActorId()) {
		throw std::logic_error("Soldier cannot attack itself");
	}

	// Ensure that the actor to attack is on an opponent team
	if (p_attack_target->GetPlayerId() == this->soldier->GetPlayerId()) {
		throw std::logic_error("Cannot attack own unit");
	}

	// Set the current attack target
	this->attack_target = p_attack_target;
	this->attack_target_set = true;
}

void MobilitySystem::MoveTo(physics::Vector destination) {
	this->destination = destination;
	this->destination_set = true;
	this->destination_set_in_prev_turn = true;
}

void MobilitySystem::HandleMove() {
	physics::Vector position = soldier->GetPosition();
	int64_t element_size = map->GetElementSize();

	// Get the speed at which the soldier can move
	int64_t speed = soldier->GetSpeed();

	// The actual destination to move towards, after path planning
	physics::Vector next_dest;

	// Check if the current position and the destination are in the same grid
	// space. If so, move directly towards it. Else, invoke the path planner to
	// find the next space to go to.
	std::cerr << "The destination is " << destination << std::endl;
	if (floor(position.x / element_size) ==
	        floor(destination.x / element_size) &&
	    floor(position.y / element_size) ==
	        floor(destination.y / element_size)) {
		std::cerr << "It's in the IF\n";

		// Destination is in the same grid space. Set it as next_dest
		next_dest = destination;

	} else {
		// Destination not in the same grid space. Need to use path planner
		// Convert the position and destination to offsets
		std::cerr << "It's in the ELSE\n";
		physics::Vector position_node(floor(position.x / element_size),
		                              floor(position.y / element_size));

		physics::Vector dest_node(floor(destination.x / element_size),
		                          floor(destination.y / element_size));

		// Use the path planner to get the next node (as an offset value)
		physics::Vector next_node =
		    soldier->path_planner->GetNextNode(position_node, dest_node);

		// Convert the next_node into a position value, which points to the
		// centre of the target grid element
		next_dest.x = floor((next_node.x * element_size) + (element_size / 2));
		next_dest.y = floor((next_node.y * element_size) + (element_size / 2));
	}
	std::cerr << "Next dest : " << next_dest << std::endl;

	// Navitgate to next_dest
	// Find the unit vector along the direction of movement
	physics::Vector direction_vector = next_dest - position;
	physics::Vector unit_vector =
	    direction_vector / direction_vector.magnitude();
	std::cerr << "Unit Vector : " << unit_vector << std::endl;

	// Multiply the speed (a scalar) with the unit vector to get the
	// displacement vector. Add the displacement with the current position to
	// get the new position
	physics::Vector new_position = position + (unit_vector * speed);
	std::cerr << "Position before ceiling : " << new_position << std::endl;
	new_position.x = ceil(new_position.x);
	new_position.y = ceil(new_position.y);
	soldier->SetPosition(new_position);
}

void MobilitySystem::HandleAttack() {
	int64_t soldier_attack_damage = soldier->GetAttackDamage();

	// Inflict damage on opponent
	std::cerr << "Target HP : " << attack_target->GetHp() << std::endl;
	attack_target->SetHp(
	    std::max((long)0, attack_target->GetHp() - soldier_attack_damage));
	std::cerr << "Target HP after : " << attack_target->GetHp() << std::endl;
}

void MobilitySystem::Update() {

	// Check if the target is in range
	bool target_in_range = false;
	if (this->attack_target_set) {
		target_in_range = this->IsAttackTargetInRange();
	}

	// Execute state code
	switch (this->soldier->GetState()) {
	case SoldierState::IDLE:
		// Do nothing
		break;

	case SoldierState::MOVE:
		HandleMove();
		break;

	case SoldierState::ATTACK:
		HandleAttack();
		break;

	case SoldierState::DEAD:
		// Do nothing
		break;
	}

	// Handle state transitions
	switch (this->soldier->GetState()) {

	case SoldierState::IDLE:
		// If destination set or attack target out of range, go to move
		if (destination_set) {
			soldier->SetState(SoldierState::MOVE);

		} else if (attack_target_set && !target_in_range) {
			destination = attack_target->GetPosition();
			soldier->SetState(SoldierState::MOVE);
			destination_set = true;

			// If attack target in range, go to attack
		} else if (attack_target_set && target_in_range) {
			soldier->SetState(SoldierState::ATTACK);

			// Else, stay idle
		} else {
			// Nothing
		}
		break;

	case SoldierState::MOVE:
		if (destination_set_in_prev_turn) {
			attack_target_set = false;
			destination_set_in_prev_turn = false;
		}

		// Arrived at destination
		if (soldier->GetPosition() == destination) {
			if (attack_target_set) {
				soldier->SetState(SoldierState::ATTACK);
			} else {
				soldier->SetState(SoldierState::IDLE);
			}
			destination_set = false;
			destination_set_in_prev_turn = false;
			break;
		}

		// If attack target not in range, move towards it
		if (attack_target_set && !target_in_range) {
			destination = attack_target->GetPosition();
			destination_set = true;

			// If attack target in range, go to attack
		} else if (attack_target_set && target_in_range) {
			soldier->SetState(SoldierState::ATTACK);

			// Else, stay in move state
		} else {
			// Nothing
		}
		break;

	case SoldierState::ATTACK:
		if (destination_set_in_prev_turn) {
			attack_target_set = false;
			destination_set_in_prev_turn = false;
			soldier->SetState(SoldierState::MOVE);
		}

		// If there's nothing set or the target is dead, go to idle
		if ((!attack_target_set) ||
		    (attack_target_set && attack_target->GetHp() == 0)) {

			attack_target_set = false;
			soldier->SetState(SoldierState::IDLE);

			// If target goes out of range, move towards it
		} else if (attack_target_set && !target_in_range) {
			soldier->SetState(SoldierState::MOVE);
		}

		break;

	case SoldierState::DEAD:
		// Do nothing. Respawn system handles this.
		break;
	}
}
}
