/**
 * @file actor.h
 * Declares the Base Actor class that all actors derive from
 */

#ifndef STATE_ACTOR_ACTOR_H
#define STATE_ACTOR_ACTOR_H

#include <cstdint>
#include <memory>
#include "state/utilities.h"
#include "physics/vector.h"
#include "state/state_export.h"
#include "state/interfaces/i_updatable.h"

namespace state {
/**
 * Actor base class
 */
class STATE_EXPORT Actor : public IUpdatable {
protected:
	/**
	 * Unique ID of the actor
	 */
	ActorId id;

	/**
	 * ID of the player that the actor belongs to
	 */
	PlayerId player_id;

	/**
	 * Actor Type
	 */
	ActorType actor_type;

	/**
	 * Current HP of the Actor
	 */
	int64_t hp;

	/**
	 * Maximum possible HP of the actor
	 */
	int64_t max_hp;

	/**
	 * Actor position
	 */
	physics::Vector position;

public:
	Actor();

	Actor(
		ActorId id,
		PlayerId player_id,
		ActorType actor_type,
		int64_t hp,
		int64_t max_hp,
		physics::Vector position
	);

	virtual ~Actor();

	/**
	 * Get actor id
	 *
	 * @return     ActorId of the actor
	 */
	ActorId GetActorId();

	/**
	 * Get the id of the player the actor belongs to
	 *
	 * @return     PlayerId of the actor
	 */
	PlayerId GetPlayerId();

	/**
	 * Get the actor type
	 *
	 * @return     PlayerType of the Actor
	 */
	ActorType GetActorType();

	/**
	 * Get the current HP of the Actor
	 *
	 * @return     Actor HP
	 */
	int64_t GetHp();

	/**
	 * Get the maximum hp of the actor
	 *
	 * @return     Actor Max HP
	 */
	int64_t GetMaxHp();

	/**
	 * Get the position of the actor
	 *
	 * @return     Actor Position
	 */
	physics::Vector GetPosition();
};

}

#endif
