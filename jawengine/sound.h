#pragma once

#include "types.h"

namespace sound {
	constexpr size_t MAX_NUM_SOUNDS = 256;

	jaw::soundid create();
	bool write(jaw::soundid id, int16_t *data, size_t size, bool loop);
	void start(jaw::soundid);
	void stop(jaw::soundid);
}