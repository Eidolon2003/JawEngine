/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../JawEngine.h"

// A simple-as-possible profiling library
// Define JAW_NPROFILE to disable profiling in a release build!

namespace profile {
	// Clear all data, use at beginning or end of frame
	inline void clear(jaw::properties*);

	// Begin a new timing period
	inline void begin(uint16_t);

	// End a previously begun timing period
	inline void end(uint16_t);

	// Get the value in nanoseconds for a completed timing period
	// You may also lookup in the periods array directly 
	inline jaw::nanoseconds get(uint16_t);

#ifndef JAW_NPROFILE
	inline jaw::nanoseconds periods[65536];
#endif

	inline void clear() {
#ifndef JAW_NPROFILE
		for (size_t i = 0; i < 65536; i++) {
			periods[i] = 0;
		}
#endif
	}

	inline void begin(uint16_t id) {
#ifndef JAW_NPROFILE
		periods[id] = util::getTimePoint();
#endif
	}

	inline void end(uint16_t id) {
#ifndef JAW_NPROFILE
		periods[id] = util::getTimePoint() - periods[id];
#endif
	}

	inline jaw::nanoseconds get(uint16_t id) {
#ifndef JAW_NPROFILE
		return periods[id];
#else
		return 0;
#endif
	}
}