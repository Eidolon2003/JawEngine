#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>	// HWND
#include "../types.h"	// jaw::properties	

namespace draw {
	void init(const jaw::properties*, HWND);
	void deinit();
	void prepareRender();
	void render();
	void present();
}
