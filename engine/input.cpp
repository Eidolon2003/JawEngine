#include "input.h"

jaw::Input::Input() {
	mouseXY = { 0, 0 };
	charInput = "";

	memset(keybits, 0, sizeof(keybits));
}

std::pair<int, int> jaw::Input::getMouseXY() {
	return mouseXY;
}

std::string jaw::Input::getString() {
	return charInput;
}

bool jaw::Input::isKeyPressed(uint8_t vkc) {
	return keybits[vkc >> 4] & 1 << (vkc & 0xF);
}