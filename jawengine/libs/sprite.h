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

#pragma once
#include "../JawEngine.h"

namespace sprite {
	constexpr size_t MAX_NUM = 1024;
	typedef uint32_t id;

	struct Sprite;
	typedef void (*sprfn)(sprite::Sprite *, jaw::properties *);

	struct Sprite {
		// physics
		jaw::vec2f pos;
		jaw::vec2f vel;	// velocity in pixels-per-second

		// animation
		jaw::vec2i fullSize;	// Size of the full bitmap
		jaw::vec2i frameSize;	// Size of one animation frame
		jaw::nanoseconds animationInterval;	// zero = not animated
		unsigned animationRow;	// choose which row of the sprite sheet is used

		// drawing
		jaw::bmpid bmp;
		uint8_t z;
		float scale;

		// custom behavior
		sprite::sprfn updateAndDraw;
		void *data;

		// Engine managed
		jaw::nanoseconds age;
		jaw::nanoseconds _lastAnimationAge;
		unsigned _animationCounter;

		// helpers
		jaw::recti rect() const {
			return jaw::recti(
				jaw::vec2i(pos),
				jaw::vec2i(pos) + (frameSize * scale)
			);
		}
	};

	// Create a new sprite
	// return jaw::INVALID_ID upon failure
	sprite::id create(const sprite::Sprite &);

	// Destroy a sprite if it exists
	void destroy(sprite::id);

	// Return a pointer to a sprite by its ID
	// Returns nullptr for an invalid ID
	sprite::Sprite *idtoptr(sprite::id);

	// Destroy all sprites
	void clear();

	// Default updateAndDraw function
	// Used automatically when a custom one isn't specified at creation time
	void updateAndDraw(sprite::Sprite *, jaw::properties *);
}