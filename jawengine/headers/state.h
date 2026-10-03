/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../JawEngine.h"

/*
	Each frame, the engine will call the active state's loop function.

	When the stack is changed using push or pop,
	the old state's deinit, and the new state's init will be called
	before calling the new state's loop.

	If two states are pushed within the same frame, for example,
	then deinit will only be called on the original, and init only on the second.
	In other words, at most only one deinit and init will be called per-frame
*/

namespace state {
	constexpr size_t MAX_NUM_STATES = 256;
	constexpr size_t MAX_STACK_SIZE = 256;

	// Create a new state, does not affect the current stack
	// Calls the new state's initOnce now
	// Returns jaw::INVALID_ID on failure
	jaw::stateid create(jaw::properties *props, const jaw::stateFns &fns);

	// Push a new state onto the stack
	bool push(jaw::stateid);

	// Pop the current state off the stack and return to the previous state
	bool pop();

	// Returns the id of the current state
	jaw::stateid current();

	// Returns the id of the previous state
	jaw::stateid previous();

	// Arena allocator for state-local data
	// Automatically cleared by the engine after a state deinits but before the next inits
	// Meaning: if you allocate in your state's init, it will exist for the life of the state
	inline util::arenaAllocator *allocator;
}