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

// Profiler

#include "headers/types.h"
#include <JawEngine.h>
#include <libs/profile.h>

static void loop(jaw::properties *props) {
	profile::begin(0);
	util::accurateSleep(jaw::millis(0.69420), util::getTimePoint());
	profile::end(0);

	char *str = util::frameAllocator->alloc<char>(256);
	float ms = jaw::to_millis(profile::get(0));
	std::snprintf(str, 256, "%f", ms);
	draw::enqueue<draw::str>({
		.str = str,
		.color = jaw::color::WHITE,
		.rect = jaw::recti(jaw::vec2i(), props->size)
	}, 0);
}

int main() {
	jaw::properties p;
	engine::start(&p, { .loop = loop });
}