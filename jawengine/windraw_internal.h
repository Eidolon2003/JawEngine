#pragma once
#include <windows.h>

namespace draw {
	void init(const jaw::properties*, HWND);
	void deinit();
	void prepareRender();
	void render();
}