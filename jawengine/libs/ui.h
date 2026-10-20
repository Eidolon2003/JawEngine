/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../JawEngine.h"
#include "cam.h"

namespace ui {
	constexpr size_t MAX_NUM = 128;
	typedef uint32_t id;
	typedef void (*uifn)(ui::id, jaw::properties*);

	constexpr size_t JAWUI_TEXT_CAPACITY = 32;
	struct UIElement {
		// The text displayed in the UI element
		// Applies to: TextDisplay, TextButton, TextInput
		char text[JAWUI_TEXT_CAPACITY] = "";

		// The UI element's logical location
		// Applies to: all
		jaw::recti rect = jaw::recti();

		// The font used to draw text
		// Applies to: TextDisplay, TextButton, TextInput
		jaw::fontid font = 0;
		
		// Colors used for drawing
		// Applies to: all
		jaw::argb backColor = jaw::color::BLACK;
		jaw::argb borderColor = jaw::color::WHITE;
		jaw::argb textColor = jaw::color::WHITE;

		// An optional camera that transforms the logical position
		// for the purposes of drawing and mouse collision detection.
		// Note: This is not automatically cleared when calling ui::clear
		// Applies to: all
		cam::id camera = jaw::INVALID_ID;

		// Callback called when the UI element is "selected"
		// Applies to: TextButton, TextInput, Checkbox
		ui::uifn select = nullptr;

		// Callback called when the UI element is "deselected"
		// Applies to: TextInput, Checkbox
		ui::uifn deselect = nullptr;

		// Set when the UI element is "selected"
		// Applies to: TextInput, Checkbox
		bool selected = false;

		// Sets the z-height of the element for drawing
		// Applies to: all
		uint8_t z;

		// Opaque data pointer for general user purposes
		// Untouched by the library
		void *data = nullptr;

		// IDs of the internal callback, clickable, and transformedRect which drive the UI
		// This should generally not be touched by the user
		jaw::callbackid cb = jaw::INVALID_ID;
		jaw::clickableid click = jaw::INVALID_ID;
		jaw::recti transformedRect;
	};

	// Destroy all UI Elements and their associated components
	void clear();
	
	// Same as clear, but destroys ALL callbacks and clickables
	// This is faster than destroying only those associated with UI Element objects,
	// but clearing everything may not be desirable
	void clearAll();

	// Destroy and clean up any UI Element
	void destroy(ui::id);

	// Return a pointer to an existing UI Element, if it exists
	// returns nullptr if not
	UIElement *idtoptr(ui::id i);

	// Functions for creating specific UI Elements
	ui::id createTextDisplay(const UIElement &e);
	ui::id createTextButton(const UIElement &e);
	ui::id createTextInput(const UIElement &e);
	ui::id createCheckbox(const UIElement &e);

	// Helper functions for basic screen layout
	// Computes the coordinates of a rect relative to the screen size
	jaw::recti relrect(jaw::vec2i screenSize, jaw::vec2f reltl, jaw::vec2f reldim);
	// Same as relrect, but forces the rect to be square
	jaw::recti relsqr(jaw::vec2i screenSize, jaw::vec2f reltl, float reldim);
}