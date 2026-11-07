/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

#include <JawEngine.h>
#include <libs/cam.cpp>

// Draw a 10x10 red/blue checkerboard, color tiles green when hovered over
// Use a camera to convert between coordinate spaces

static cam::id camera;

void init(jaw::properties *) {
	// (0, 0) is offset 100 pixels from the top left in both dimensions
	// Each tile is 25x25 pixels
	camera = cam::create({ .offset={100,100}, .multiplier=25 });
}

void loop(jaw::properties *) {
	// Get the mouse's coordinates in tile-space
	jaw::vec2i hovered_tile = cam::reverse(camera, input::getMouse().pos);

	// Draw a grid of tiles using tile coordinate space
	// The camera converts from tile-space to screen-space
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			auto tile_rect = jaw::recti(x, y, x+1, y+1);

			jaw::argb color;
			if (tile_rect.tl == hovered_tile) {
				color = jaw::color::GREEN;
			}
			else {
				color = (x ^ y) & 1 ? jaw::color::RED : jaw::color::BLUE;
			}

			draw::enqueue<draw::rect>({
				.rect = cam::forward(camera, tile_rect),
				.color = color
			}, 0);
		}
	}
}

int main() {
	jaw::properties props;
	engine::start(&props, { .init = init, .loop = loop });
}