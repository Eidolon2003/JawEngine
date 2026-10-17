/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

// Demonstrate basic gamepad handling
// NOTE: at this time, we only support Sony controllers.
// Specifically, this code has been tested on DualShock 4 and DualSense.

#include <JawEngine.h>

// This ID represents the read state, as defined by the read function below
static jaw::stateid readState = jaw::INVALID_ID;

static jaw::fontid textFont = jaw::INVALID_ID;

static void read_loop(jaw::properties *props) {
	// Get a pointer to the "player 0" controller
	const jaw::gamepad *gamepad = input::getGamepad(0);

	// If the controller doesn't exist, go back to the polling state.
	// Perhaps it was disconnected!
	if (gamepad == nullptr) {
		state::pop();
		return;	// This return saves us from dereferencing a nullptr below
	}

	// Draw the left analog stick on the screen
	const jaw::vec2i center = props->size / 2;
	jaw::vec2f offset = { 200, 200 };
	offset = offset * gamepad->sony.l;
	const jaw::vec2i pos = center + jaw::vec2i(offset);
	draw::enqueue<draw::rect>({
		.rect = jaw::recti(pos - 5, pos + 5),
		.color = jaw::color::WHITE
	}, 0);
}

// This function is run once when the initial state is created,
// which happens during engine startup
static void first_initOnce(jaw::properties *props) {
	// Create the read state
	readState = state::create(props, { .loop = read_loop });

	// Create a font for drawing large text centered in the window
	textFont = draw::newFont({
		.name = "Comic Sans MS",
		.size = 32.0f,
		.alignx = draw::font::CENTERX,
		.aligny = draw::font::CENTERY
	});
}

static void first_init(jaw::properties *props) {
	// Poll for gamepads every 100ms
	callback::create({
		.timeInterval = jaw::millis(100),
		.callback = [](jaw::callbackid, jaw::properties*) {
			input::findNewGamepads();
		}
	});
}

static void first_deinit(jaw::properties *props) {
	// Stop the callback we created in first_init
	callback::clear();
}

static void first_loop(jaw::properties *props) {
	// Go to the button reading state if we find a gamepad
	if (input::getGamepad(0)) state::push(readState);

	draw::enqueue<draw::str>({
		.rect = jaw::recti({}, props->size),
		.str = "Searching for a compatible gamepad...",
		.color = jaw::color::WHITE,
		.font = textFont
	}, 0);
}

int main() {
	jaw::properties p;

	// Gives text a smoother look, if you aren't going for a pixelated look
	p.enableSubpixelTextRendering = true;

	engine::start(&p, {
		.initOnce = first_initOnce,
		.loop = first_loop,
		.init = first_init,
		.deinit = first_deinit
	});
}