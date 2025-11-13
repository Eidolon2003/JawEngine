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

#include "../state.h"
#include "internal_state.h"

struct functions {
	jaw::statefn init, loop;
};
static_assert(std::is_trivial_v<functions>);

static functions states[state::MAX_NUM_STATES];
static size_t numStates;

static jaw::stateid stack[state::MAX_STACK_SIZE];
static size_t stackTop;

static bool newStateFlag;

jaw::stateid state::create(jaw::properties *props, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop) {
	if (numStates == state::MAX_NUM_STATES
		|| loop == nullptr
	) {
		return jaw::INVALID_ID;
	}
	jaw::stateid s = (jaw::stateid)numStates++;
	states[s] = { init, loop };
	if (initOnce) initOnce(props);
	return s;
}

bool state::push(jaw::stateid id) {
	if (stackTop == state::MAX_STACK_SIZE
		|| id == jaw::INVALID_ID
	) {
		return false;
	}
	newStateFlag = true;
	stack[stackTop++] = id;
	return true;
}

bool state::pop() {
	if (stackTop == 0) return false;
	newStateFlag = true;
	stackTop--;
	return true;
}

jaw::stateid state::top() {
	if (stackTop == 0) return jaw::INVALID_ID;
	else return stack[stackTop-1];
}

bool state::loop(jaw::properties *props) {
	if (stackTop == 0) return false;
	const jaw::stateid s = stack[stackTop - 1];
	const functions &f = states[s];

	if (newStateFlag && f.init) {
		newStateFlag = false;
		f.init(props);
	}
	f.loop(props);
	return true;
}

void state::deinit() {
	newStateFlag = false;
	numStates = 0;
	stackTop = 0;
}