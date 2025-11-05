#pragma once
#include <windows.h>
#include "../types.h"

namespace input {
	void init(HWND hwnd);
	void deinit();
	void readGamepads();
}
