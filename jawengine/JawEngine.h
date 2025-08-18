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
#include "asset.h"
#include "draw.h"
#include "input.h"
#include "state.h"

namespace engine {
	void start(jaw::properties*, state::fptr initOnce, state::fptr init, state::fptr loop);
	void stop();
}