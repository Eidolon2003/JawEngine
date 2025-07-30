#pragma once
#include "types.h"

namespace input {
	void init();
	void deinit();
	void beginFrame();
	void updateMouse(const jaw::mouse*);
	void updateChar(char);
	void updateKey(uint8_t code, bool isDown);
}