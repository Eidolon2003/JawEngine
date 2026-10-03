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

#include "../jawengine/JawEngine.h"
#include <iostream>

// This demos the state-local allocator
// The starting state allocates an integer and starts it at zero.
// each loop, it is incremented by one.

// When the 'a' key is pressed, a new state is pushed (and popped the frame after)
// This has the effect of clearing the state allocator and resetting the counter
// (note that bytesUsed does not go up each time 'a' is pressed)

static int *ptr;
static jaw::stateid popper;

void popper_loop(jaw::properties *) {
	state::pop();
}

void initOnce(jaw::properties *props) {
	popper = state::create(props, { .loop = popper_loop });
}

void init(jaw::properties *) {
	// This allocates 7 bytes because of alloc's alignment logic
	ptr = state::allocator->alloc<int>(1);
	*ptr = 0;

	input::bindKeyDown(key::A, [](jaw::properties *) { state::push(popper); });
}

void loop(jaw::properties *) {
	*ptr = *ptr + 1;
	std::cout << *ptr << ", " << state::allocator->bytesUsed() << std::endl;

	// Allocate from the frameAllocator every frame
	// This will allocate a max of 1537 bytes
	void *a = util::frameAllocator->allocRaw(1537);
}

void deinit(jaw::properties *) {
	input::clear();
}

int main() {
	jaw::properties props;
	props.showCMD = true;

	engine::start(&props, { .initOnce = initOnce, .init = init, .loop = loop });
}