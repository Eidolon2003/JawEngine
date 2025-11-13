/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025 Julian Williams
 *
 * JawEngine 0.2.0
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../types.h"

namespace input {
	void beginFrame(jaw::properties*);
	void updateMouse(const jaw::mouse*, jaw::properties *props);
	void updateChar(char);
	void updateKey(uint8_t code, bool isDown, jaw::properties *props);
}