#pragma once
#define NOMINMAX
#include <windows.h>
#include "types.h"

namespace draw {
	void init(const jaw::properties*, HWND);
	void deinit();
	void prepareRender();
	void render();
}