#pragma once
#include "types.h"

namespace state {
	constexpr size_t MAX_NUM_STATES = 256;
	constexpr size_t MAX_STACK_SIZE = 256;

	// Returns jaw::INVALID_ID on failure
	jaw::stateid create(jaw::properties *props, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop);
	bool push(jaw::stateid);
	bool pop();
}