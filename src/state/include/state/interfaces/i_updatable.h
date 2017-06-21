/**
 * @file i_updatable.h
 * Declares the updatable interfacs, which gives classs an update method
 */

#ifndef STATE_INTERFACES_I_UPDATABLE
#define STATE_INTERFACES_I_UPDATABLE

#include "state/state_export.h"

/**
 * IUpdatable interface to define the Update method
 */
class STATE_EXPORT IUpdatable {
public:
	virtual ~IUpdatable() {};
	virtual void Update() = 0;
};

#endif
