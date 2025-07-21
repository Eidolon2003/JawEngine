#pragma once
#include <windows.h>

namespace draw {
	void init(jaw::properties*, HWND);
	void deinit();
	void prepareRender();
	void render();
}