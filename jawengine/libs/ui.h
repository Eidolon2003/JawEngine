/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../JawEngine.h"

#ifndef JAWUI_TEXT_CAPACITY
#define JAWUI_TEXT_CAPACITY 32
#endif

#ifndef JAWUI_TEXT_DISPLAY_BORDER_WIDTH
#define JAWUI_TEXT_DISPLAY_BORDER_WIDTH 2
#endif

#ifndef JAWUI_TEXT_BUTTON_DESELECT_BORDER_WIDTH
#define JAWUI_TEXT_BUTTON_DESELECT_BORDER_WIDTH 2
#endif

#ifndef JAWUI_TEXT_BUTTON_SELECT_BORDER_WIDTH
#define JAWUI_TEXT_BUTTON_SELECT_BORDER_WIDTH 3
#endif

#ifndef JAWUI_TEXT_INPUT_DESELECT_BORDER_WIDTH
#define JAWUI_TEXT_INPUT_DESELECT_BORDER_WIDTH 2
#endif

#ifndef JAWUI_TEXT_INPUT_SELECT_BORDER_WIDTH
#define JAWUI_TEXT_INPUT_SELECT_BORDER_WIDTH 3
#endif

#ifndef JAWUI_CHECKBOX_BORDER_WIDTH
#define JAWUI_CHECKBOX_BORDER_WIDTH 2
#endif

#ifndef JAWUI_CHECKBOX_STROKE_WIDTH
#define JAWUI_CHECKBOX_STROKE_WIDTH 2
#endif

namespace ui {
	constexpr size_t MAX_NUM = 128;
	typedef uint32_t id;
	typedef void (*uifn)(ui::id, jaw::properties*);

	struct UIElement {
		char text[JAWUI_TEXT_CAPACITY] = "";
		jaw::recti rect = jaw::recti();
		jaw::fontid font = 0;
		jaw::argb backColor = jaw::color::BLACK;
		jaw::argb borderColor = jaw::color::WHITE;
		jaw::argb textColor = jaw::color::WHITE;
		jaw::callbackid cb = jaw::INVALID_ID;
		jaw::clickableid click = jaw::INVALID_ID;
		ui::uifn select = nullptr, deselect = nullptr;
		bool selected = false;
		void *data = nullptr;
		uint8_t z;
	};

	// Destroy all UI Elements and their associated components
	void clear();
	
	// Same as clear, but destroys ALL callbacks and clickables
	// This is faster than destroying only those associated with UI Element objects,
	// but clearing everything may not be desirable
	void clearAll();

	// Destroy and clean up any UI Element
	void destroy(ui::id);

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