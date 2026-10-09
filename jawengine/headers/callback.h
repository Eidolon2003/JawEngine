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

#pragma once
#include "types.h"

namespace callback {
	constexpr size_t MAX_NUM_CALLBACKS = 256;

	// Create a new callback in callback's slotAllocator
	// Returns jaw::INVALID_ID if out of space
	jaw::callbackid create(const jaw::callback &cb);

	// Destroy a callback if it exists
	void destroy(jaw::callbackid id);

	// Return a pointer the the callback data associated with its ID
	// Returns nullptr on invalid ID
	jaw::callback *idtoptr(jaw::callbackid id);

	// Destroy all callbacks
	void clear();
}