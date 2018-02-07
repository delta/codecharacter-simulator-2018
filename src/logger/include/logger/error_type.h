/**
 * file error_type.h
 * Defines a type to represent a particular category of errors
 */

#ifndef LOGGER_ERROR_TYPE_H
#define LOGGER_ERROR_TYPE_H

#include <string>
#include <vector>

namespace logger {

/**
 * Enum representing different categories of errors. Errors from the same
 * category can potentially be grouped
 *
 * ALERT! - When adding a value here, add it in the string mapping below as well
 */
enum class ErrorType {
	/**
     * Trying to perform multiple soldier commands
     */
	NO_MULTIPLE_SOLDIER_TASKS,

	/**
     * Trying to perform multiple tower commands
     */
	NO_MULTIPLE_TOWER_TASKS,

	/**
     * Trying to change the actor_id property
     */
	NO_ALTER_ACTOR_ID,

	/**
     * Trying to command a dead soldier
     */
	NO_ACTION_BY_DEAD_SOLDIER,

	/**
     * Trying to attack a base tower
     */
	NO_ATTACK_BASE_TOWER,

	/**
     * Trying to move a soldier to an invalid position
     */
	INVALID_POSITION,

	/**
     * Trying to attack one's own tower
     */
	NO_ATTACK_SELF_TOWER,

	/**
     * Trying to attack one's own soldier
     */
	NO_ATTACK_SELF_SOLDIER,

	/**
     * Trying to attack a dead soldier
     */
	NO_ATTACK_DEAD_SOLDIER,

	/**
     * Trying to build on an invalid territory
     */
	INVALID_TERRITORY,

	/**
     * Trying to build or upgrade with insufficient money
     */
	INSUFFICIENT_FUNDS,

	/**
     * Trying to upgrade a tower already at the max level
     */
	NO_MORE_UPGRADES,

	/**
     * Trying to suicide the base tower
     */
	NO_SUICIDE_BASE_TOWER,

	/**
     * Trying to build tower after reaching maximum limit
     */
	NO_MORE_TOWERS,

	/**
     * Trying to attack tower that will be razed in same turn by opponent
     */
	NO_ATTACK_RAZED_TOWER,

	/**
     * Trying to attack soldier that is currently invulnerable
     */
	NO_ATTACK_IMMUNE_SOLDIER
};

/**
 * Provides an easy way to get the string representation of the above enum
 */
const std::vector<std::string> ErrorTypeName = {
    "NO_MULTIPLE_SOLDIER_TASKS", "NO_MULTIPLE_TOWER_TASKS",
    "NO_ALTER_ACTOR_ID",         "NO_ACTION_BY_DEAD_SOLDIER",
    "NO_ATTACK_BASE_TOWER",      "INVALID_POSITION",
    "NO_ATTACK_SELF_TOWER",      "NO_ATTACK_SELF_SOLDIER",
    "NO_ATTACK_DEAD_SOLDIER",    "INVALID_TERRITORY",
    "INSUFFICIENT_FUNDS",        "NO_MORE_UPGRADES",
    "NO_SUICIDE_BASE_TOWER",     "NO_MORE_TOWERS",
    "NO_ATTACK_RAZED_TOWER",     "NO_ATTACK_IMMUNE_SOLDIER"};
}

#endif
