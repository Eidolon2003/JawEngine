#pragma once
#include "types.h"

namespace input {
	constexpr size_t MAX_INPUT_STRING = 256;

	const jaw::mouse* getMouse();
	jaw::key getKey(uint8_t code);

	// Adds characters to this string upto size
	// May also remove characters if the backspace key was pressed
	void getString(char* str, size_t);
}