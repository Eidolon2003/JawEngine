#define _CRT_SECURE_NO_WARNINGS
#include "input.h"
#include "internal_input.h"
#include <string>	//strlen, strncat
#include <cassert>

static jaw::mouse mouse;
static char inputString[256];
static size_t inputStringLength;
static jaw::key keys[256];
static bool keysReleased[256];
unsigned backspaceCount;

void input::init() {
	mouse = {};
}

void input::deinit() {}

void input::beginFrame() {
	for (int i = 0; i < 256; i++) {
		if (keysReleased[i]) {
			keys[i].isDown = false;
			keysReleased[i] = false;
		}
		keys[i].isHeld = keys[i].isDown;
	}

	backspaceCount = 0;
	mouse.wheelDelta = 0;
	mouse.prevFlags = mouse.flags;
	inputString[0] = 0;
	inputStringLength = 0;
}

void input::updateMouse(const jaw::mouse* m) {
	mouse.wheelDelta += m->wheelDelta;
	mouse.flags.all = m->flags.all;
	mouse.pos = m->pos;
}

void input::updateChar(char c) {
	if (c == '\b') {
		if (inputStringLength == 0) backspaceCount++;
		else inputString[--inputStringLength] = 0;
		return;
	}

	inputString[inputStringLength++] = c;
	assert(inputStringLength < MAX_INPUT_STRING);
	inputString[inputStringLength] = 0;
}

// This logic is to make sure the game sees isDown being true
// even if the key was pressed and released within one frame
void input::updateKey(uint8_t code, bool isDown) {
	if (isDown) {
		keys[code].isDown = true;
		keysReleased[code] = false;
		assert(keys[code].isHeld == false);
	}
	else {
		if (keys[code].isHeld) {
			keys[code].isDown = false;
			keys[code].isHeld = false;
			assert(keysReleased[code] == false);
		}
		else {
			keysReleased[code] = true;	// Pressed and released in the same frame
			assert(keys[code].isDown == true);
			assert(keys[code].isHeld == false);
		}
	}
}

const jaw::mouse* input::getMouse() { 
	return &mouse;
}

// Remove characters from the input string if the backspace key was pressed,
// then add new ones
void input::getString(char* str, size_t size) {
	size_t len = strlen(str);
	for (size_t i = 0; i < backspaceCount && len > 0; i++) {
		str[--len] = 0;
	}
	strncat(str, inputString, size);
}

jaw::key input::getKey(uint8_t code) {
	return keys[code];
}