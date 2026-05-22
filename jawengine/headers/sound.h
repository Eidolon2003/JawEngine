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


	// Defaults to standard A=440Hz, equal temperament
	struct ABCOptions {
		union {
			float freqs[12] = {
				261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f,
				369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f
			};
			struct {
				float c, db, d, eb, e, f, gb, g, ab, a, bb, b;
			};
		};

		bool loop = true;

		// Between 0 and 1
		// Lower values = more aggressive low pass filter
		// Higher values = rawer, brighter sound
		float lowpass = 0.33f;

		// Multiplication factor applied at the very end
		// Beware of clipping!
		float masterGain = 1.f;
	};

	// Parses ABC and synthesizes audio data!
	jaw::soundid abc(const char *abcData, const ABCOptions &opt);
}