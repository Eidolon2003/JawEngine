#include "input.h"

jaw::Input::Input(bool repeat) {
	enableKeyRepeat = repeat;
	mouseXY = { 0, 0 };
	charInput = "";
	memset(keybits, 0, sizeof(keybits));
	std::fill(downJumpTable, downJumpTable + TABLELEN, nullptr);
	std::fill(upJumpTable, upJumpTable + TABLELEN, nullptr);
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

void jaw::Input::BindKeyDown(uint8_t vkc, std::function<void()> f) {
	downJumpTable[vkc] = f;
}

void jaw::Input::BindKeyUp(uint8_t vkc, std::function<void()> f) {
	upJumpTable[vkc] = f;
}