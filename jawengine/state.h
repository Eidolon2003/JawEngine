#pragma once
#include "types.h"

namespace state {
	constexpr size_t MAX_NUM_STATES = 256;
	constexpr size_t MAX_STACK_SIZE = 256;

	jaw::stateid create(jaw::properties* props, jaw::fptr initOnce, jaw::fptr init, jaw::fptr loop);
	bool push(jaw::stateid);
	bool pop();
}