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

// Simple sprite demo

#include <JawEngine.h>
#include <libs/sprite.h>

// Relative path from the build output dir to the source
#define BMP_PATH "../../../../../examples/example07_sprites/NPC_test.png"

// Defines which row in the sprite sheet corresponds with which direction
enum : unsigned {
	ANIM_MOVE_DOWN,
	ANIM_MOVE_RIGHT,
	ANIM_MOVE_UP,
	ANIM_MOVE_LEFT
};

static sprite::id mySprite;

static void init(jaw::properties *) {
	// Load the bitmap asset
	jaw::vec2i bitmap_size;
	jaw::argb *pixels = asset::bmp(BMP_PATH, &bitmap_size);
	if (!pixels) return;

	// Create the bitmap with the draw API
	jaw::bmpid bitmap = draw::createBmp(bitmap_size);
	draw::writeBmp(bitmap, pixels, bitmap_size);

	// Create the sprite and give it the bitmap
	mySprite = sprite::create({
		.fullSize = bitmap_size,
		.frameSize = {16, 32},	// Each frame is 16x32 pixels
		.bmp = bitmap,
		.scale = 3,
		.animationInterval = jaw::millis(250)	// Change animation frame every 250ms
	});

	draw::setBackgroundColor(jaw::color::DARK_MAGENTA);
}

// Move the sprite around the screen with WASD, animate in the direction of movement
static void loop(jaw::properties *props) {
	// Get the pointer to the sprite back using the ID
	sprite::Sprite *spr = sprite::idtoptr(mySprite);
	if (!spr) return;

	constexpr float SPEED = 200.0f;
	spr->vel = {};
	if (input::getKey(key::W).isHeld) {
		spr->animationNextRow = spr->animationRow = ANIM_MOVE_UP;
		spr->vel = spr->vel + jaw::vec2f(0, -SPEED);
	}
	if (input::getKey(key::S).isHeld) {
		spr->animationNextRow = spr->animationRow = ANIM_MOVE_DOWN;
		spr->vel = spr->vel + jaw::vec2f(0, +SPEED);
	}
	if (input::getKey(key::A).isHeld) {
		spr->animationNextRow = spr->animationRow = ANIM_MOVE_LEFT;
		spr->vel = spr->vel + jaw::vec2f(-SPEED, 0);
	}
	if (input::getKey(key::D).isHeld) {
		spr->animationNextRow = spr->animationRow = ANIM_MOVE_RIGHT;
		spr->vel = spr->vel + jaw::vec2f(+SPEED, 0);
	}
}

int main() {
	jaw::properties props;
	engine::start(&props, { .init = init, .loop = loop });
}

#include <libs/sprite.cpp>