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

// Sprite demo

#include <JawEngine.h>
#include <libs/sprite.h>

static void init(jaw::properties *props) {
	jaw::vec2i size;
	jaw::argb *pixels = asset::bmp("F:\\assets\\test-animation\\idle-40x70x6.png", &size);
	if (pixels == nullptr) return;

	jaw::bmpid bitmap = draw::createBmp(size);
	draw::writeBmp(bitmap, pixels, size);

	(void)sprite::create({
		.pos = {50, 50},
		.bmp = bitmap,
		.fullSize = size,
		.frameSize = {40, 70},
		.scale = 5,
		.animationInterval = jaw::millis(300)
	});
}

int main() {
	jaw::properties p;
	engine::start(&p, { .init = init });
}

#include <libs/sprite.cpp>