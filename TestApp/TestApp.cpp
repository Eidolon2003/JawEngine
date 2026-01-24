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

void loop(jaw::properties *p) {
	draw::enqueue(draw::rect{
		.rect = jaw::recti(p->mouse.pos, p->mouse.pos + 50),
		.color = 0xAAFFFFFF
	}, 0);
}

int main() {
	for (;;) {
		jaw::properties props;
		engine::start(&props, nullptr, nullptr, loop);
	}
}
