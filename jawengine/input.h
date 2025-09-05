#pragma once
#include "types.h"

namespace input {
	constexpr size_t MAX_INPUT_STRING = 256;
	constexpr size_t MAX_NUM_CLICKABLE = 256;

	jaw::key getKey(uint8_t code);

	// Adds characters to this string upto size
	// May also remove characters if the backspace key was pressed
	void getString(char* str, size_t);

	// Clear all key and mouse bindings, and all clickables
	void clear();

	void bindKeyDown(uint8_t code, jaw::statefn);
	void bindKeyUp(uint8_t code, jaw::statefn);

	// These are called when a mouse event is not already handled by a clickable
	void bindLMBDown(jaw::statefn);
	void bindLMBUp(jaw::statefn);
	void bindRMBDown(jaw::statefn);
	void bindRMBUp(jaw::statefn);
	void bindMMBDown(jaw::statefn);
	void bindMMBUp(jaw::statefn);
	void bindXMB1Down(jaw::statefn);
	void bindXMB1Up(jaw::statefn);
	void bindXMB2Down(jaw::statefn);
	void bindXMB2Up(jaw::statefn);

	// Returns jaw::INVALID_ID on failure
	jaw::clickableid createClickable(const jaw::clickable&);

	// Destroys a clickable if it exists
	void destroy(jaw::clickableid);

	// Returns nullptr on invalid ID
	jaw::clickable* idtoptr(jaw::clickableid);
}