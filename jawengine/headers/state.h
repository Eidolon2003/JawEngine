/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025 Julian Williams
 *
 * JawEngine 0.2.0
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "types.h"

namespace state {
	constexpr size_t MAX_NUM_STATES = 256;
	constexpr size_t MAX_STACK_SIZE = 256;

	// Create a new state, does not affect the current stack
	// Calls the new state's initOnce now
	// Returns jaw::INVALID_ID on failure
	jaw::stateid create(jaw::properties *props, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop);

	// Push a new state onto the stack, calling the new state's init
	bool push(jaw::stateid);

	// Pop the current state off the stack and return to the previous state, calling init on the new state
	bool pop();

	// Returns the id of the current state
	jaw::stateid top();
}