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

// Basic camera usage as it stands

#include <JawEngine.h>
#include <libs/cam.cpp>
#include <iostream>

constexpr int16_t BOARD_SIZE = 10;
static cam::id mapcam;

void mouseHandler(jaw::properties *) {
	// Convert screen to map to get which tile we clicked on
	auto pos = input::getMouse().pos;
	jaw::vec2i boardPos = cam::reverse(mapcam, pos);
	std::cout << "Clicked (" << boardPos.x << ", " << boardPos.y << ")\n";
}

void init(jaw::properties *) {
	// Converting a map coordinate to a screen coordinate involves:
	//    multiplying by 30, because each tile is 30x30 px
	//    adding the offset from the corner 
	mapcam = cam::create({
		.multiplier = 30.0f,
		.offset = {100, 30}
	});

	input::bindLMBDown(mouseHandler);
}

void deinit(jaw::properties *) {
	cam::clear();
	input::clear();
}

void loop(jaw::properties *) {
	// Draw a checkerboard using the camera for positioning
	for (int16_t x = 0; x < BOARD_SIZE; x++) {
		for (int16_t y = 0; y < BOARD_SIZE; y++) {
			// converting from map to screen is forward by convention
			auto screentl = cam::forward(mapcam, jaw::vec2i(x, y));
			auto screenbr = cam::forward(mapcam, jaw::vec2i(x+1, y+1));
			draw::enqueue<draw::rect>({
				.rect = jaw::recti(screentl, screenbr),
				.color = (x + y) & 1 ? jaw::color::RED : jaw::color::BLUE
			}, 0);
		}
	}
}

int main() {
	jaw::properties p;
	p.showCMD = true;
	engine::start(&p, { .loop = loop, .deinit = deinit, .init = init });
}