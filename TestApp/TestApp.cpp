/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

// General arena use

#include <JawEngine.h>

static void loop(jaw::properties *props) {
	// Allocate a 1K arena every frame
	auto arena = util::arenaAllocator(1<<10);

	// Do arena things
	int *arr = arena.alloc<int>(100);
	arr[99] = 69;

	char *frameArr = util::frameAllocator->alloc<char>(1000);
	frameArr[568] = 'x';

	// The arena is freed at the end of the scope
}

int main() {
	jaw::properties p;
	engine::start(&p, { .loop = loop });
}