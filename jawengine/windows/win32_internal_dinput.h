#pragma once
#include <Windows.h>
#include "../types.h"

namespace input {
	void init(HWND hwnd);
	void deinit();
	void readGamepads();
}