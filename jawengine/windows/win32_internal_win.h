#pragma once
#define NOMINMAX
#include <windows.h>	// HWND
#include "../types.h"	// jaw::properties

namespace win {
	HWND init(jaw::properties*);
	void deinit();
}