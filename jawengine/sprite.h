#pragma once
#include "types.h"

namespace sprite {
	constexpr size_t MAX_NUM_SPR = 65536;

	// Clear all sprites
	void clear();

	// Create a new sprite with given parameters
	// Returns jaw::INVALID_ID if out of space
	jaw::sprid create(const jaw::sprite&);

	// Destroy a sprite, freeing up its slot, if it exists
	void destroy(jaw::sprid);

	// Convert sprite ID to a pointer for reading or writing
	// Returns nullptr on invalid ID
	jaw::sprite* idtoptr(jaw::sprid);

	// Assign a custom update function to a sprite
	// By default, the default handler is called
	// To extend the default handler rather than replacing it, call it in your custom funciton
	void customUpdate(jaw::sprid, jaw::sprfn);

	// Assign a custom draw function to a sprite
	// By default, the default handler is called
	// To extend the default handler rather than replacing it, call it in your custom funciton
	void customDraw(jaw::sprid, jaw::sprfn);

	// Default handler: handles age and moving pos by vel
	void update(jaw::sprid, jaw::properties*);

	// Default handler: draws the spr's bitmap at pos if it exists
	void draw(jaw::sprid, jaw::properties*);
}

namespace anim {
	constexpr size_t MAX_NUM_ANIM = 65536;

	// Clears all animation definitions and states
	void clear();

	// Create a new type of animation which can be reused
	// Returns jaw::INVALID_ID if out of space
	jaw::animdefid create(const jaw::animation&);

	// Convert IDs to pointers
	// Returns nullptr for invalid ID
	jaw::animation* idtoptr(jaw::animdefid);

	// Create an instance of an animation type to tie to one or more sprites
	// Returns jaw::INVALID_ID if out of space
	jaw::animstateid instanceOf(jaw::animdefid);

	// Destroys an animation state returned by instanceOf
	// Does NOT destroy an animation definition, those live forever.
	void destroy(jaw::animstateid);

	// Returns whether or not a non-looped animation has finished
	// Returns false on invalid ID on looped animations
	bool finished(jaw::animstateid);

	// Returns which frame the given animation is currently on
	// Returns UINT32_MAX on invalid ID
	uint32_t getFrame(jaw::animstateid);
}