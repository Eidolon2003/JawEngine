#pragma once
#include "types.h"

namespace input {
	constexpr size_t MAX_INPUT_STRING = 256;
	constexpr size_t MAX_NUM_CLICKABLE = 256;
	constexpr size_t MAX_GAMEPADS = 16;

	// Return the current state of a keyboard key
	jaw::key getKey(uint8_t code);

	// Adds characters to this string upto size
	// May also remove characters if the backspace key was pressed
	void getString(char *str, size_t);

	// Clear all key and mouse bindings, and all clickables
	void clear();

	// The engine will automatically call your callback when the given key is pressed
	void bindKeyDown(uint8_t code, jaw::statefn);

	// The engine will automatically call your callback when the given key is released
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

	// Register a clickable to get notified when it is clicked on
	// Returns jaw::INVALID_ID on failure
	jaw::clickableid createClickable(const jaw::clickable&);

	// Destroys a clickable if it exists
	void destroy(jaw::clickableid);

	// Returns nullptr on invalid ID
	jaw::clickable *idtoptr(jaw::clickableid);

	// Returns the number of connected and initialized gamepads
	unsigned numGamepads();

	// Initialize newly connected gamepads
	// returns true if a new gamepad was found
	// This is relatively slow, so it should be done sparingly
	bool findNewGamepads();

	// Get the gamepad in numbered slot
	// Gamepads fill in the lowest available slot starting from zero
	// If a gamepad is disconnected, its slot will become available
	// Returns nullptr for empty slots
	const jaw::gamepad *getGamepad(unsigned index);
}