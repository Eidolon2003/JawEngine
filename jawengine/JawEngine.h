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
#include "utils.h"
#include "asset.h"
#include "draw.h"

#ifndef JAW_NSPRITE
#include "sprite.h"
#endif

#ifndef JAW_NSOUND
#include "sound.h"
#endif

#ifndef JAW_NINPUT
#include "input.h"
#endif

#ifndef JAW_NSTATE
#include "state.h"
#endif

namespace engine {
	void start(jaw::properties*, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop);
	void stop();
}