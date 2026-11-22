/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../JawEngine.h"

// A simple-as-possible profiling library
// JAW_PROFILE is enabled when compiling in profile mode

namespace profile {
	// Clear all data, use at beginning or end of frame
	void clear(jaw::properties*);

	// Begin a new timing period
	void begin(uint8_t);

	// End a previously begun timing period
	void end(uint8_t);

	// Get the value in nanoseconds for a completed timing period
	// You may also lookup in the periods array directly 
	jaw::nanoseconds get(uint8_t);
}