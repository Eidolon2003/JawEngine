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

#include "../jawengine/JawEngine.h"

char yes[] = "This CPU supports AVX2";
char no[] = "This CPU does not support AVX2";

void loop(jaw::properties *p) {
	draw::enqueue(draw::str{
		.rect = jaw::recti(0,0,240,180),
		.str = p->cpuid.avx2 ? yes : no,
		.color = jaw::color::WHITE
	}, 0);
}

int main() {
	jaw::properties props;
	props.targetFramerate = 1000;
	props.scale = 3;
	props.size.x = 240;
	props.size.y = 180;
	props.title = "AVX2 Test";
	jaw::start(&props, nullptr, nullptr, loop);
}
