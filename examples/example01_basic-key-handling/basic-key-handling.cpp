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

// Move a character around the window with WASD

#include <JawEngine.h>
#include <algorithm>	// std::clamp

// The character is a 20x20 rectangle
constexpr jaw::vec2i CHARACTER_SIZE {20, 20};

// The character's speed
// When multiplied directly with jaw::to_seconds(props->totalFrame),
// this value is in "pixels per second".
constexpr float CHARACTER_SPEED = 250.0f;

// Storing the character's position in floating point
// This is the top-left corner of the character rect
static jaw::vec2f characterPos;

// init is run once when the state is entered
static void init(jaw::properties *props) {
	// Set up a keybinding that closes the window when ESC is pressed
	input::bindKeyDown(key::ESC, [](jaw::properties *) { engine::stop(); });
}

// deinit is run once when the state is exited
static void deinit(jaw::properties *props) {
	// Always remember to clean up after yourself!
	// Anything that your state sets up in init should be cleaned up in deinit
	input::clear();

	// This isn't necessarily important with only one state like we have here,
	// but it's required for managing multiple states
}

static void loop(jaw::properties *props) {
	// Handle key inputs for movement
	if (input::getKey(key::W).isHeld) {
		characterPos.y -= CHARACTER_SPEED * jaw::to_seconds(props->totalFrametime);
	}
	if (input::getKey(key::S).isHeld) {
		characterPos.y += CHARACTER_SPEED * jaw::to_seconds(props->totalFrametime);
	}
	if (input::getKey(key::A).isHeld) {
		characterPos.x -= CHARACTER_SPEED * jaw::to_seconds(props->totalFrametime);
	}
	if (input::getKey(key::D).isHeld) {
		characterPos.x += CHARACTER_SPEED * jaw::to_seconds(props->totalFrametime);
	}

	// Don't allow the character to go out-of-bounds
	characterPos.x = std::clamp<float>(characterPos.x, 0, props->size.x - CHARACTER_SIZE.x);
	characterPos.y = std::clamp<float>(characterPos.y, 0, props->size.y - CHARACTER_SIZE.y);

	// Draw the character as a red rectangle
	// Here we convert the position from float to int for drawing
	draw::enqueue<draw::rect>({
		.rect = jaw::recti(jaw::vec2i(characterPos), jaw::vec2i(characterPos) + CHARACTER_SIZE),
		.color = jaw::color::RED
	}, 0);
}

int main() {
	jaw::properties p {
		.size = {800, 600},
	};

	// Tell the engine to use our loop, init, and deinit functions in the initial state
	engine::start(&p, { .loop = loop, .init = init, .deinit = deinit });
}