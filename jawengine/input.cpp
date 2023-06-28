#include "input.h"

jaw::Input::Input(bool repeat) {
	enableKeyRepeat = repeat;
	mouse = { 0, 0, 0, 0 };
	charInput.clear();
	memset(keybits, 0, sizeof(keybits));
	std::fill(downJumpTable, downJumpTable + TABLELEN, nullptr);
	std::fill(upJumpTable, upJumpTable + TABLELEN, nullptr);
	clickDown = nullptr;
	clickUp = nullptr;
}

void jaw::Input::Reset() {
	charInput.clear();
}

jaw::Input::Mouse jaw::Input::getMouse() {
	return mouse;
}

std::wstring jaw::Input::getString() {
	return charInput;
}

bool jaw::Input::isKeyPressed(uint8_t vkc) {
	return keybits[vkc >> 4] & 1 << (vkc & 0xF);
}

void jaw::Input::BindKeyDown(uint8_t vkc, const std::function<void()>& f) {
	downJumpTable[vkc] = f;
}

void jaw::Input::BindKeyUp(uint8_t vkc, const std::function<void()>& f) {
	upJumpTable[vkc] = f;
}

void jaw::Input::BindClickDown(const std::function<void(Mouse)>& f) {
	clickDown = f;
}

void jaw::Input::BindClickUp(const std::function<void(Mouse)>& f) {
	clickUp = f;
}