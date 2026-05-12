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

// Do this to turn off profiling in release mode
#ifdef NDEBUG
#define JAW_NPROFILE
#endif

#include "../jawengine/JawEngine.h"
#include "../jawengine/libs/profile.h"
#include <thread>	//sleep_for

enum : uint16_t { PROFILE_TEST };

static void loop(jaw::properties *p) {
	profile::clear();
	profile::begin(PROFILE_TEST);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	profile::end(PROFILE_TEST);

	char *buf = util::tempalloc<char>(16);
	snprintf(buf, 16, "%.2f", jaw::to_millis(profile::get(PROFILE_TEST)));
	buf[15] = '\0';
	draw::enqueue(draw::str{
		.rect = jaw::recti({}, p->size),
		.str = buf,
		.color = jaw::color::WHITE
	}, 0);
}

int main() {
	jaw::properties props;
	props.targetFramerate = 30;
	props.scale = 20;
	props.size.x = 40;
	props.size.y = 30;
	props.title = "Profiler";
	engine::start(&props, nullptr, nullptr, loop); 
}