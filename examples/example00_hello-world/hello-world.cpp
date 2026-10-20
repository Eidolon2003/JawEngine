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

 // Draw the string "Hello, world!" to a graphical window!

#include <JawEngine.h>

// This function will be called by the engine every frame
// (The window is cleared at the start of each frame)
static void loop(jaw::properties *props) {

	// This function queues up a draw call for the renderer to process at the end of the frame
	draw::enqueue<draw::str>({
		.rect = jaw::recti({10, 10}, props->size),
		.color = jaw::color::WHITE,
		.str = "Hello, world!"
	}, 0);	// This final parameter is the draw call's z height
}

int main() {
	// The engine requires a properties struct, so we will allocate it here.
	// At startup, the properties struct is used to define a few useful settings
	jaw::properties p {
		.size = {160, 120},		// The logical window size, 160x120 pixels
		.scale = 5,				// Multiplies the logical size by 5 to produce an 800x600 window
		.targetFramerate = 0,	// A target framerate of 0 indicates the use of Vsync
		.title = "Hello"		// Sets the title that appears in the window's title bar
	};

	// This is the main entry point to the engine.
	// It takes a pointer to the properties struct we defined previously,
	// and a set of functions that make up the initial state.
	// In this case, we're setting up a loop funciton, which will be called every frame.
	engine::start(&p, { .loop = loop });
}

// A note on style:
// I prefer to use designated initializers to interact with various APIs in the engine.
// The project mandates clang, which allows out-of-order designated initializers in C++20, to improve the ergonomics of this style.
// However, you are not required to use designated initializers if you'd prefer not to.