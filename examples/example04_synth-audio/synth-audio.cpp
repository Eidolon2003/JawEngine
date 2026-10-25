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

#include "../jawengine/JawEngine.h"

// Demo the ABC synth. This audio is generated at runtime from the source text :)
// See: https://abcnotation.com/, https://abcnotation.com/wiki/abc:standard:v2.1

const char *abc = "\
%abc                                                             \n\
X:1                                                              \n\
T:Matoya                                                         \n\
V:1 jaw-waveform=square   jaw-amplitude=5000  jaw-lowpass=0.15   \n\
V:2 jaw-waveform=square   jaw-amplitude=4000  jaw-lowpass=0.15   \n\
V:3 jaw-waveform=triangle jaw-amplitude=15000 jaw-lowpass=0.7    \n\
M:4/4                                                            \n\
L:1/8                                                            \n\
Q:1/4=150                                                        \n\
K:D                                                              \n\
%                                                                \n\
V:1                                                              \n\
BdfB g2 fe | d2 ed c4 | dfad b2 ag | f2 gf e2 fg |               \n\
V:2                                                              \n\
B,FB,F B,GB,G | B,GB,G CACA | DADA DBDB | DBDB CACA |            \n\
V:3                                                              \n\
B,,4 E,4 | G,4 A,2 G,2 | D,4 G,4 | B,4 A,.E,.C,.A,, |            \n\
%                                                                \n\
V:1                                                              \n\
a4 c2 e2 | d2/3c2/3d2/3 B4 ga | b4 d2 g2 | f2/3g2/3f2/3 e4 ab |  \n\
V:2                                                              \n\
AA x2 AA x2 | FF x2 FF x2 | BB x2 BB x2 | cc x2 cc x2 |          \n\
V:3                                                              \n\
xF,,.C,.F, x.F,,F,.C, | xB,,.F,.B, x.B,,B,.F, |                  \n\
xG,,.D,.G, x.G,,G,.D, | xA,,.E,.A, xA,,A,.E, |                   \n\
%                                                                \n\
V:1                                                              \n\
c'4 xabc' | e'/d'/c'/d'/ b3agf | e5 =cde | d4 c2 A2 |            \n\
V:2                                                              \n\
AA x2 AA x2 | FF x2 FF x2 | EGEG EGEG | EGEG A/G/F/E/ F/E/D/C/ | \n\
V:3                                                              \n\
xF,,.C,.F, x.F,,F,.C, | xB,,.F,.B, x.B,,B,.F, |                  \n\
=C,.=C =C,.=C =C,.=C =C,.=C | A,,.A, A,,.A, A,,.A, A,,.A,        \n\
%                                                                \n\
V:1                                                              \n\
d8 | d6 d/c/B/c/ | d8 | d6 f/e/d/e/ |                            \n\
V:2                                                              \n\
F2 D2 G2 D2 | AADA G2 E2 | F2 D2 G2 D2 | AADA G2 E2 |            \n\
V:3                                                              \n\
D,.A,x.A, D,.B,x.B, | D,.Cx.C D,.B,x.B, |                        \n\
D,.A,x.A, D,.B,x.B, | D,.Cx.C D,.B,x.B, |                        \n\
%                                                                \n\
V:1                                                              \n\
f8 | f6 a/g/f/g/ | a8 | a4 f/e/d/c/ e/d/c/^A/ |                  \n\
V:2                                                              \n\
F2 D2 G2 D2 | AADA G2 E2 | F2 D2 G2 D2 | AADA G2 E2 |            \n\
V:3                                                              \n\
D,.A,x.A, D,.B,x.B, | D,.Cx.C D,.B,x.B, |                        \n\
D,.A,x.A, D,.B,x.B, | D,.Cx.C F,,.C, F,,.C, |                    \n\
";

void init(jaw::properties *) {
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
	jaw::properties props;
	engine::start(&props, { .init = init });
}