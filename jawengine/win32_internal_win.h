#pragma once
#define NOMINMAX
#include <windows.h>
#include "types.h"

namespace win {
	HWND init(jaw::properties*);
	void deinit();
}