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
	
/*
	HEADER
*/
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


/*
	IMPLEMENTATION
*/
	inline util::slotAllocator<id, UIElement, MAX_NUM> slots;

	inline void clear() {
		for (ui::id i = 0; i < slots.nextSlot; i++) {
			if (slots.isOpen[i]) continue;

			ui::id id = i | slots.gens[i];
			ui::destroy(id);
		}
		slots.clear();
	}

	inline void clearAll() {
		input::clear();
		callback::clear();
		slots.clear();
	}

	inline void destroy(ui::id x) {
		UIElement *e = slots.idtoptr(x);
		if (!e) [[unlikely]] return;
		if (e->click != jaw::INVALID_ID) input::destroy(e->click);
		if (e->cb != jaw::INVALID_ID) callback::destroy(e->cb);
		slots.destroy(x);
	}

	inline UIElement *idtoptr(ui::id i) {
		return slots.idtoptr(i);
	}

	inline id createTextDisplay(const UIElement &e)
	{
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				constexpr int16_t BORDER = JAWUI_TEXT_DISPLAY_BORDER_WIDTH;
				draw::drawCall calls[3]{
					draw::make<draw::rect>({
						.rect = p->rect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->rect.tl + BORDER, p->rect.br - BORDER),
						.color = p->backColor
					}, p->z),
					draw::make<draw::str>({
						.rect = jaw::recti(p->rect.tl + BORDER, p->rect.br - BORDER),
						.str = p->text,
						.color = p->textColor,
						.font = p->font
					}, p->z)
				};
				draw::enqueueMany(calls, 3);
			}
		});
		if (cbid == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		slots.idtoptr(x)->cb = cbid;
		return x;
	}

	inline id createTextButton(const UIElement &e)
	{
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				constexpr int16_t DESELECT_BORDER = JAWUI_TEXT_BUTTON_DESELECT_BORDER_WIDTH;
				constexpr int16_t SELECT_BORDER = JAWUI_TEXT_BUTTON_SELECT_BORDER_WIDTH;
				int16_t border = p->rect.contains(input::getMouse().pos) ? SELECT_BORDER : DESELECT_BORDER;

				draw::drawCall calls[3]{
					draw::make<draw::rect>({
						.rect = p->rect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
						.color = p->backColor
					}, p->z),
					draw::make<draw::str>({
						.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
						.str = p->text,
						.color = p->textColor,
						.font = p->font
					}, p->z)
				};
				draw::enqueueMany(calls, 3);
			}
		});
		if (cbid == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->cb = cbid;

		jaw::clickableid click = input::createClickable(jaw::clickable{
			.rect = &ep->rect,
			.condition = jaw::mouseFlags{.lmb = true },
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				if (p->select) p->select(id, props);
			}
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			callback::destroy(cbid);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		return x;
	}

	inline id createTextInput(const UIElement &e) {
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				if (p->selected && (
					(input::getMouse().flags.lmb && !p->rect.contains(input::getMouse().pos)) ||
					(input::getKey(key::ENTER).isDown && !input::getKey(key::SHIFT).isHeld)
					)
					) {
					p->selected = false;
					if (p->deselect) p->deselect(id, props);
				}

				if (p->selected) input::getString(p->text, JAWUI_TEXT_CAPACITY);

				constexpr int16_t DESELECT_BORDER = JAWUI_TEXT_INPUT_DESELECT_BORDER_WIDTH;
				constexpr int16_t SELECT_BORDER = JAWUI_TEXT_INPUT_SELECT_BORDER_WIDTH;
				int16_t border = p->selected ? SELECT_BORDER : DESELECT_BORDER;

				draw::drawCall calls[3]{
					draw::make<draw::rect>({
						.rect = p->rect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
						.color = p->backColor
					}, p->z),
					draw::make<draw::str>({
						.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
						.str = p->text,
						.color = p->textColor,
						.font = p->font
					}, p->z)
				};
				draw::enqueueMany(calls, 3);
			}
		});
		if (cbid == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->cb = cbid;

		jaw::clickableid click = input::createClickable(jaw::clickable{
			.rect = &ep->rect,
			.condition = jaw::mouseFlags{.lmb = true },
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->selected = true;
				if (p->select) p->select(id, props);
			}
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			callback::destroy(cbid);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		return x;
	}

	inline id createCheckbox(const UIElement &e) {
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				constexpr int16_t BORDER = JAWUI_CHECKBOX_BORDER_WIDTH;
				constexpr int16_t STROKE = JAWUI_CHECKBOX_STROKE_WIDTH;
				draw::drawCall calls[4] = {
					draw::make<draw::rect>({
						.rect = p->rect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->rect.tl + BORDER, p->rect.br - BORDER),
						.color = p->backColor
					}, p->z),
					draw::make<draw::line>({
						.p1 = p->rect.tl + 2*BORDER,
						.p2 = p->rect.br - 2*BORDER,
						.color = p->textColor,
						.width = STROKE
					}, p->z)
				};
				draw::enqueueMany(calls, 2 + p->selected);
			}
		});
		if (cbid == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->cb = cbid;

		jaw::clickableid click = input::createClickable(jaw::clickable{
			.rect = &ep->rect,
			.condition = jaw::mouseFlags{.lmb = true },
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->selected = !p->selected;
			}
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			callback::destroy(cbid);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		return x;
	}

	inline jaw::recti relrect(jaw::vec2i screenSize, jaw::vec2f reltl, jaw::vec2f reldim) {
		auto tl = jaw::vec2i(jaw::vec2f(screenSize) * reltl);
		auto br = tl + jaw::vec2i(jaw::vec2f(screenSize) * reldim);
		return jaw::recti(tl, br);
	}

	inline jaw::recti relsqr(jaw::vec2i screenSize, jaw::vec2f reltl, float reldim) {
		auto tl = jaw::vec2i(jaw::vec2f(screenSize) * reltl);
		float dim = std::min(screenSize.x, screenSize.y) * reldim;
		auto br = tl + jaw::vec2i(jaw::vec2f(dim, dim));
		return jaw::recti(tl, br);
	}
}