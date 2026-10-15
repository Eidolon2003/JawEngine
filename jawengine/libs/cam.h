/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../JawEngine.h"

// By convention, cam::forward converts a non-sreen coordinate to a screen coordinate.
// This is because one of the main use-cases for a camera is "drawing through it".
// Drawing through a camera should look like: draw(cam::forward(id, v));

namespace cam {
	constexpr size_t MAX_NUM = 128;
	typedef uint32_t id;

	struct Camera {
		float multiplier;
		jaw::vec2i offset;
	};
	static_assert(std::is_trivial_v<Camera>);

	// Clear all cameras
	void clear();

	// Create a new camera, returning its ID
	// Returns jaw::INVALID_ID when out of space
	cam::id create(const Camera&);

	// Destroy a camera by ID if it exists
	void destroy(cam::id);

	// Return a pointer to the camera identified by its ID
	// Returns nullptr if the camera no longer exists
	Camera *idtoptr(cam::id);

	// Transform a vec forward through the camera into a new vec
	jaw::vec2i forward(cam::id, jaw::vec2i);

	// Transform a vec backward through the camera
	// v == reverse(forward(v))
	jaw::vec2i reverse(cam::id, jaw::vec2i);
};