#define _CRT_SECURE_NO_WARNINGS
#include "../input.h"
#include "internal_input.h"
#include <cstring>	//strlen, strncat
#include <cassert>

static char inputString[input::MAX_INPUT_STRING];
static size_t inputStringLength;
static jaw::key keys[256];
static unsigned backspaceCount;

static jaw::statefn keyDownBindings[256];
static jaw::statefn keyUpBindings[256];
static jaw::statefn lmbDown, lmbUp, rmbDown, rmbUp, mmbDown,
					mmbUp, xmb1Down, xmb1Up, xmb2Down, xmb2Up;

static jaw::clickable clickables[input::MAX_NUM_CLICKABLE];
static jaw::clickableid openSlots[input::MAX_NUM_CLICKABLE];
static bool isOpen[input::MAX_NUM_CLICKABLE];
static size_t nextID;
static size_t numOpen;


void input::beginFrame(jaw::properties *props) {
	for (int i = 0; i < 256; i++) {
		keys[i].isDown = false;
	}

	backspaceCount = 0;
	props->mouse.wheelDelta = 0;
	inputString[0] = 0;
	inputStringLength = 0;
}

void input::updateMouse(const jaw::mouse *m, jaw::properties *props) {
	props->mouse.wheelDelta += m->wheelDelta;
	props->mouse.prevFlags.all = props->mouse.flags.all;
	props->mouse.flags.all = m->flags.all;
	props->mouse.pos = m->pos;

	jaw::mouseFlags changed;
	changed.all = props->mouse.flags.all ^ props->mouse.prevFlags.all;
	if (changed.all == 0) return;

	// First check clickables
	for (size_t i = 0; i < nextID; i++) {
		if (isOpen[i]) continue;

		jaw::clickable *c = clickables + i;

		// Check if modifier keys match condition
		if (c->condition.shift != m->flags.shift ||
			c->condition.ctrl != m->flags.ctrl) continue;

		// Check if at least one of the button conditions match
		jaw::mouseFlags buttonMask;
		buttonMask.all = m->flags.all & c->condition.all;
		buttonMask.ctrl = buttonMask.shift = 0;
		if ((buttonMask.all & c->condition.all) == 0) continue;

		// Get the clickable's rectangle
		if (!c->getRect) continue;
		jaw::recti rect = c->getRect(props);

		// Check the click falls within the rect
		if (!rect.contains(m->pos)) continue;

		if (c->callback) {
			c->callback(props);
			return;
		}
	}

	// If clickables don't match, call bound handler(s)
	if (changed.lmb) {
		if (m->flags.lmb) {
			if (lmbDown) lmbDown(props);
		}
		else {
			if (lmbUp) lmbUp(props);
		}
	}

	if (changed.rmb) {
		if (m->flags.rmb) {
			if (rmbDown) rmbDown(props);
		}
		else {
			if (rmbUp) rmbUp(props);
		}
	}

	if (changed.mmb) {
		if (m->flags.mmb) {
			if (mmbDown) mmbDown(props);
		}
		else {
			if (mmbUp) mmbUp(props);
		}
	}

	if (changed.xmb1) {
		if (m->flags.xmb1) {
			if (xmb1Down) xmb1Down(props);
		}
		else {
			if (xmb1Up) xmb1Up(props);
		}
	}

	if (changed.xmb2) {
		if (m->flags.xmb2) {
			if (xmb2Down) xmb2Down(props);
		}
		else {
			if (xmb2Up) xmb2Up(props);
		}
	}
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

// isHeld tracks if the key is being held or not 
// isDown is true for one frame when the key is pressed (rising edge trigger)

// This logic (along with resetting isDown to false at the beginning of each frame),
// maintains that isDown is true even if the key was pressed and released on the same frame.
// isHeld will not be true if the key was pressed and release in the same frame.
void input::updateKey(uint8_t code, bool isDown, jaw::properties *props) {
	if (isDown) {
		keys[code].isDown = true;
		keys[code].isHeld = true;
		if (keyDownBindings[code]) keyDownBindings[code](props);
	}
	else {
		keys[code].isHeld = false;
		if (keyUpBindings[code]) keyUpBindings[code](props);
	}
}

// Remove characters from the input string if the backspace key was pressed,
// then add new ones
void input::getString(char *str, size_t size) {
	size_t len = strlen(str);
	for (size_t i = 0; i < backspaceCount && len > 0; i++) {
		str[--len] = 0;
	}
	strncat(str, inputString, size);
}

jaw::key input::getKey(uint8_t code) {
	return keys[code];
}

void input::bindKeyDown(uint8_t code, jaw::statefn f) { keyDownBindings[code] = f; }

void input::bindKeyUp(uint8_t code, jaw::statefn f) { keyUpBindings[code] = f; }

void input::bindLMBDown(jaw::statefn fn) { lmbDown = fn; }

void input::bindLMBUp(jaw::statefn fn) { lmbUp = fn; }

void input::bindRMBDown(jaw::statefn fn) { rmbDown = fn; }

void input::bindRMBUp(jaw::statefn fn) { rmbUp = fn; }

void input::bindMMBDown(jaw::statefn fn) { mmbDown = fn; }

void input::bindMMBUp(jaw::statefn fn) { mmbUp = fn; }

void input::bindXMB1Down(jaw::statefn fn) { xmb1Down = fn; }

void input::bindXMB1Up(jaw::statefn fn) { xmb1Up = fn; }

void input::bindXMB2Down(jaw::statefn fn) { xmb2Down = fn; }

void input::bindXMB2Up(jaw::statefn fn) { xmb2Up = fn; }

void input::clear() {
	memset(keyDownBindings, 0, sizeof(keyDownBindings));
	memset(keyUpBindings, 0, sizeof(keyUpBindings));
	lmbDown = lmbUp = rmbDown = rmbUp = mmbDown = mmbUp = xmb1Down = xmb1Up = xmb2Down = xmb2Up = nullptr;

	nextID = 0;
	numOpen = 0;
	memset(isOpen, 0, sizeof(isOpen));
}

jaw::clickableid input::createClickable(const jaw::clickable &c) {
	if (nextID == input::MAX_NUM_CLICKABLE && numOpen == 0) return jaw::INVALID_ID;

	jaw::clickableid newID;
	if (numOpen > 0) {
		newID = openSlots[--numOpen];
		isOpen[newID] = false;
	}
	else {
		newID = (jaw::clickableid)nextID++;
	}

	memcpy(clickables + newID, &c, sizeof(jaw::clickable));
	return newID;
}

void input::destroy(jaw::clickableid id) {
	if (id >= nextID || isOpen[id]) return;
	isOpen[id] = true;
	openSlots[numOpen++] = id;
}

jaw::clickable *input::idtoptr(jaw::clickableid id) {
	if (id >= nextID || isOpen[id]) return nullptr;
	return clickables + id;
}
