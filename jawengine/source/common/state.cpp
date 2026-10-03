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

#include "../../headers/state.h"
#include "internal_state.h"

static jaw::stateFns states[state::MAX_NUM_STATES];
static size_t numStates;

static jaw::stateid stack[state::MAX_STACK_SIZE];
static size_t stackTop;

static bool newStateFlag;
static jaw::stateid prevState = jaw::INVALID_ID;

#ifndef NDEBUG
static size_t maxBytes;
#endif

void state::init(jaw::properties *props) {
	static auto _stateAllocator = util::arenaAllocator(props->stateAllocatorBytes);
	allocator = &_stateAllocator;
}

jaw::stateid state::create(jaw::properties *props, const jaw::stateFns &fns) {
	if (numStates == state::MAX_NUM_STATES ||
		fns.loop == nullptr)
	{
		return jaw::INVALID_ID;
	}
	jaw::stateid s = (jaw::stateid)numStates++;
	states[s] = fns;
	if (fns.initOnce) fns.initOnce(props);
	return s;
}

bool state::push(jaw::stateid id) {
	if (stackTop == state::MAX_STACK_SIZE ||
		id == jaw::INVALID_ID)
	{
		return false;
	}

	if (newStateFlag == false) {
		newStateFlag = true;
		if (stackTop > 0) prevState = stack[stackTop - 1];
		else prevState = jaw::INVALID_ID;
	}

	stack[stackTop++] = id;
	return true;
}

bool state::pop() {
	if (stackTop == 0) return false;

	if (newStateFlag == false) {
		newStateFlag = true;
		if (stackTop < state::MAX_STACK_SIZE) prevState = stack[stackTop - 1];
		else prevState = jaw::INVALID_ID;
	}

	stackTop--;
	return true;
}

jaw::stateid state::current() {
	if (stackTop == 0) return jaw::INVALID_ID;
	else return stack[stackTop-1];
}

jaw::stateid state::previous() {
	if (prevState == jaw::INVALID_ID) return jaw::INVALID_ID;
	else return prevState;
}

bool state::loop(jaw::properties *props) {
	if (stackTop == 0) return false;
	const jaw::stateid currentState = stack[stackTop - 1];
	const jaw::stateFns &currentFns = states[currentState];

	// If this is a new state, call the old one's deinit and the new one's init
	if (newStateFlag) {
		newStateFlag = false;

		if (prevState != jaw::INVALID_ID) {
			const jaw::stateFns &oldFns = states[prevState];
			if (oldFns.deinit) oldFns.deinit(props);
		}

#ifndef NDEBUG
		if (allocator->bytesUsed() > maxBytes) maxBytes = allocator->bytesUsed();
#endif
		allocator->clear();

		if (currentFns.init) currentFns.init(props);
	}

	currentFns.loop(props);
	return true;
}

void state::deinit(jaw::properties *props) {
#ifndef NDEBUG
	// Check one last time at the end in case newStateFlag was never set
	if (allocator->bytesUsed() > maxBytes) maxBytes = allocator->bytesUsed();
	JAW_DBGPRINT("state::allocator used a maximum of " << maxBytes << " bytes");
#endif

	const jaw::stateid currentState = stack[stackTop - 1];
	const jaw::stateFns &currentFns = states[currentState];
	if (currentFns.deinit) currentFns.deinit(props);

	newStateFlag = false;
	numStates = 0;
	stackTop = 0;
}