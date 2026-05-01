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

namespace sound {
	constexpr size_t MAX_NUM_SOUNDS = 256;

	// Create a new empty sound buffer
	jaw::soundid create();

	// Write a stream of samples into a sound buffer, and whether it should loop
	bool write(jaw::soundid id, int16_t *sampleData, size_t numSamples, bool loop);

	// Start playing a sound from the beginning
	void start(jaw::soundid);

	// Stop playing a sound
	void stop(jaw::soundid);

	// Stop playing all sounds
	void stopAll();
}