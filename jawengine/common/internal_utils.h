#pragma once
#include "../types.h"

namespace util {
	bool init(jaw::properties*);
	void deinit();
	void beginFrame();
	void updateTimers(jaw::properties*);
}