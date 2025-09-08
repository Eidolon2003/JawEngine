#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>	// HWND
#include "../types.h"	// jaw::properties

namespace win {
	HWND init(jaw::properties*);
	void deinit();
}
