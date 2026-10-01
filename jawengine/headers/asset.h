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
#include "types.h"
#include <string>
#include <vector>

namespace asset {
	// This function is cached internally
	// Points to a read-only memory mapped file, do not write!
	const void *file(const char *filename, size_t *size);

	// This function is cached internally
	// Supports multiple file formats
	// PNG and BMP are recommended because they're lossless and support alpha
	jaw::argb *bmp(const char *filename, jaw::vec2i *dim);

	// This function is cached internally
	// Accepts 44.1kHz, 16 bit, 2 channel wav files
	int16_t *wav(const char *filename, size_t *numSamples);


	// INI file structure example:
	/*
		;comment
		key=value

		;comment
		key=value
	*/
	struct INIEntry {
		std::string key, value, comment;

		INIEntry(const char *k, const char *v, const char *c) {
			key = std::string(k);
			value = std::string(v);
			comment = std::string(c);
		}
	};

	// Pass a vector of entries with keys you're interested in, default values, and comments
	// The function will read the ini file, update values you're interested in, and rewrite the file
	// This function is not backed by a cache like the others
	void readINI(const char *filename, std::vector<INIEntry> *vec);

	// Overwrite INI file with the contents of the vector
	void writeINI(const char *filename, std::vector<INIEntry> *vec);


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
	int16_t *abc(const char *abcData, size_t *numSamples, const ABCOptions &opt);
}