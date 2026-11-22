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
#include "profile.h"

// A simple-as-possible profiling library
// JAW_PROFILE is enabled when compiling in profile mode

namespace profile {
#ifdef JAW_PROFILE
	static jaw::nanoseconds periods[256];

	void clear() {
		memset(periods, 0, sizeof(periods));
	}

	void begin(uint8_t id) {
		periods[id] = util::getTimePoint();
	}

	void end(uint8_t id) {
		periods[id] = util::getTimePoint() - periods[id];
	}

	jaw::nanoseconds get(uint8_t id) {
		return periods[id];
	}
#else
	void clear() {}

	void begin(uint8_t id) {}

	void end(uint8_t id) {}

	jaw::nanoseconds get(uint8_t id) {
		return 0;
	}
#endif
}