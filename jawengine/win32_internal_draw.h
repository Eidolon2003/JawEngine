#pragma once
#define NOMINMAX
#include <windows.h>	// HWND
#include "types.h"		// jaw::properties	

namespace draw {
	void init(const jaw::properties*, HWND);
	void deinit();
	void prepareRender();
	void render();
	void present();
}