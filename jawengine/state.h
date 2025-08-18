#pragma once
#include "types.h"

namespace state {
	typedef void (*fptr)(jaw::properties*);

	constexpr size_t MAX_NUM_STATES = 256;
	constexpr size_t MAX_STACK_SIZE = 256;

	jaw::stateid create(jaw::properties* props, fptr initOnce, fptr init, fptr loop);
	bool push(jaw::stateid);
	bool pop();
}