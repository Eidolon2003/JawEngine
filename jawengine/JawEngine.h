#pragma once
#include <cstdint>
#include <cassert>

#if (defined _WIN32 || defined _WIN64)
#include "winvkc.h"
#elif (defined LINUX || defined __linux__)
#error "Linux support forthcoming"
#else
#error "unsupported platform"
#endif

#include "types.h"
#include "draw.h"
#include "input.h"

namespace game {
	void init();
	void loop();
}

namespace engine {
	void start(jaw::properties*);
	void stop();
	jaw::nanoseconds getUptime();
}