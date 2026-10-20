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

// Demonstrate basic mouse handling

#include <JawEngine.h>
#include <cassert>

static jaw::argb color;

// init is run once when the state is entered
static void init(jaw::properties *props) {
	// Same ESC binding as in the previous example
	input::bindKeyDown(key::ESC, [](jaw::properties *) { engine::stop(); });

	// Change the color on left click
	color = jaw::color::RED;
	input::bindLMBDown([](jaw::properties *) {
		switch (color) {
		case jaw::color::RED:
			color = jaw::color::GREEN;
			break;
		case jaw::color::GREEN:
			color = jaw::color::BLUE;
			break;
		case jaw::color::BLUE:
			color = jaw::color::RED;
			break;
		default:
			assert(false && "UNREACHABLE");
		}
	});
}

// deinit is run once when the state is exited
static void deinit(jaw::properties *props) {
	// This clears the key and mouse bindings at once now
	input::clear();
}

static void loop(jaw::properties *props) {
	// Draw a rect at the mouse's position
	jaw::mouse mouse = input::getMouse();
	draw::enqueue<draw::rect>({
		.rect = jaw::recti(mouse.pos, mouse.pos + 20),
		.color = color
	}, 0);
}

int main() {
	jaw::properties p {
		.size = {800, 600},
	};

	engine::start(&p, { .loop = loop, .init = init, .deinit = deinit });
}