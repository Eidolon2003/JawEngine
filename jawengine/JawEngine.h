#pragma once

#include <cstdint>
#include <chrono>
#include "structs.h"
#include "draw.h"

namespace game {
	void init();
	void loop();
}

namespace engine {
	void start(jaw::properties*);
	void stop();
}

#if (defined _WIN32 || defined _WIN64)
	#define JAW_WINDOWS
	#define NOMINMAX
	#include "winvkc.h"
#elif (defined LINUX || defined __linux__)
	#define JAW_LINUX
	#error "Linux support forthcoming"
#else
	#error "unsupported platform"
#endif