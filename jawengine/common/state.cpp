#include "../state.h"
#include "internal_state.h"

struct functions {
	jaw::fptr init, loop;
};

static functions states[state::MAX_NUM_STATES];
static size_t numStates = 0;

static jaw::stateid stack[state::MAX_STACK_SIZE];
static size_t stackTop = 0;

static bool newStateFlag;

jaw::stateid state::create(jaw::properties* props, jaw::fptr initOnce, jaw::fptr init, jaw::fptr loop) {
	if (numStates == state::MAX_NUM_STATES
		|| init == nullptr
		|| loop == nullptr
	) {
		return (jaw::stateid)(state::MAX_NUM_STATES);
	}
	states[numStates] = { init, loop };
	if (initOnce) initOnce(props);
	return (jaw::stateid)numStates++;
}

bool state::push(jaw::stateid id) {
	if (stackTop == state::MAX_STACK_SIZE) return false;
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

bool state::loop(jaw::properties* props) {
	if (stackTop == 0) return false;
	const jaw::stateid s = stack[stackTop - 1];
	const functions& f = states[s];

	if (newStateFlag) {
		newStateFlag = false;
		f.init(props);
	}
	f.loop(props);
	return true;
}