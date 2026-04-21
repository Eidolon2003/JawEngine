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

// Test sprite ID system
// Should count up sequentially far past the maximum number of slots

void loop(jaw::properties *p) {
	jaw::sprite spr{};
	jaw::sprid id = sprite::create(spr);
	sprite::destroy(id);
	char *buf = util::tempalloc<char>(16);
	snprintf(buf, 16, "%ul", (uint32_t)id);
	draw::enqueue(draw::str{
		.rect = jaw::recti(0, 0, 160, 120),
		.str = buf,
		.color = jaw::color::WHITE
	}, 0);
}

int main() {
	jaw::properties props;
	props.targetFramerate = 1000;
	props.scale = 4;
	props.size.x = 160;
	props.size.y = 120;
	engine::start(&props, nullptr, nullptr, loop);
}
