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
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstdint>
#include <cassert>

// The engine uses JAW_DBGPRINT for printing in debug mode, and you can too
#ifndef NDEBUG
#include <iostream>
inline constexpr const char *_JAW_FILENAME(const char *file) {
	// For engine source files, give the name with jawengine as the root
	// for other files, simply return the full path
	constexpr auto strcmp9 = [](const char *a, const char *b) -> bool {
		for (size_t i = 0; i < 9; i++) {
			if (a[i] != b[i]) return false;
		}
		return true;
	};
	for (const char *p = file; *p; p++) {
		if (strcmp9(p, "jawengine")) return p;
	}
	return file;
}
#define JAW_DBGPRINT(x) std::cerr << "jaw-dbg: \"" << x << "\"\n" << _JAW_FILENAME(__FILE__) << ", " << __LINE__ << "\n\n";
#else
#define JAW_DBGPRINT(x)
#endif

#if (defined _WIN32 || defined _WIN64)
#include "headers/winvkc.h"
#elif (defined LINUX || defined __linux__)
#error "Linux support forthcoming"
#else
#error "unsupported platform"
#endif

#include "headers/types.h"
#include "headers/utils.h"
#include "headers/asset.h"
#include "headers/draw.h"

#ifndef JAW_NSPRITE
#include "headers/sprite.h"
#endif

#ifndef JAW_NSOUND
#include "headers/sound.h"
#endif

#ifndef JAW_NINPUT
#include "headers/input.h"
#endif

#ifndef JAW_NSTATE
#include "headers/state.h"
#endif

namespace engine {
	void start(jaw::properties*, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop);
	void stop();
}