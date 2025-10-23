#pragma once
#include "../types.h"

namespace input {
	void beginFrame(jaw::properties*);
	void updateMouse(const jaw::mouse*, jaw::properties *props);
	void updateChar(char);
	void updateKey(uint8_t code, bool isDown, jaw::properties *props);
}