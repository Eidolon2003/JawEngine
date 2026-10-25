/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

// Synth testing

#include <JawEngine.h>

const char *abc = "\
%abc\n\
\n\
K:C\n\
V:1 jaw-waveform=pulse jaw-dutycycle=0.5 jaw-lowpass=0.1\n\
c\n\
V:1 jaw-lowpass=0.5\n\
c\n\
";

static void init(jaw::properties *props) {
	size_t numSamples;
	int16_t *audioData = asset::abc(abc, &numSamples, {});
	if (audioData) {
		jaw::soundid music = sound::create();
		if (music != jaw::INVALID_ID) {
			if (sound::write(music, audioData, numSamples, true))
				sound::start(music);
		}

	}	
}

int main() {
	jaw::properties p;
	engine::start(&p, { .init = init });
}