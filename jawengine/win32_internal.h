#pragma once
#define NOMINMAX
#include <windows.h>
#include "structs.h"

namespace win {
	HWND init(jaw::properties*);
	void deinit();
}