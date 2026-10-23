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
#include "ui.h"
#include "cam.cpp"	// The implementation of ui relies on the implementation of cam
// Because these cpp files are also pragma once, the user can also include cam.cpp without issue

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
	static util::slotAllocator<id, UIElement, MAX_NUM> slots;

	void clear() {
		for (ui::id i = 0; i < slots.nextSlot; i++) {
			if (slots.isOpen[i]) continue;

			ui::id id = i | slots.gens[i];
			ui::destroy(id);
		}
		slots.clear();
	}

	void clearAll() {
		input::clear();
		callback::clear();
		slots.clear();
	}

	void destroy(ui::id x) {
		UIElement *e = slots.idtoptr(x);
		if (!e) [[unlikely]] return;
		if (e->click != jaw::INVALID_ID) input::destroy(e->click);
		if (e->cb != jaw::INVALID_ID) callback::destroy(e->cb);
		slots.destroy(x);
	}

	UIElement *idtoptr(ui::id i) {
		return slots.idtoptr(i);
	}

	id createTextDisplay(const UIElement &e)
	{
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.frameInterval = 1,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->transformedRect = cam::forward(p->camera, p->rect);

				constexpr int16_t BORDER = JAWUI_TEXT_DISPLAY_BORDER_WIDTH;
				draw::drawCall calls[3]{
					draw::make<draw::rect>({
						.rect = p->transformedRect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->transformedRect.tl + BORDER, p->transformedRect.br - BORDER),
						.color = p->backColor
					}, p->z),
					draw::make<draw::str>({
						.rect = jaw::recti(p->transformedRect.tl + BORDER, p->transformedRect.br - BORDER),
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

	id createTextButton(const UIElement &e)
	{
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.frameInterval = 1,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->transformedRect = cam::forward(p->camera, p->rect);

				constexpr int16_t DESELECT_BORDER = JAWUI_TEXT_BUTTON_DESELECT_BORDER_WIDTH;
				constexpr int16_t SELECT_BORDER = JAWUI_TEXT_BUTTON_SELECT_BORDER_WIDTH;
				int16_t border = p->transformedRect.contains(input::getMouse().pos) ? SELECT_BORDER : DESELECT_BORDER;

				draw::drawCall calls[3]{
					draw::make<draw::rect>({
						.rect = p->transformedRect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->transformedRect.tl + border, p->transformedRect.br - border),
						.color = p->backColor
					}, p->z),
					draw::make<draw::str>({
						.rect = jaw::recti(p->transformedRect.tl + border, p->transformedRect.br - border),
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
			.rect = &ep->transformedRect,
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

		ep->transformedRect = cam::forward(ep->camera, ep->rect);

		return x;
	}

	id createTextInput(const UIElement &e) {
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.frameInterval = 1,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->transformedRect = cam::forward(p->camera, p->rect);

				if (p->selected && (
					(input::getMouse().flags.lmb && !p->transformedRect.contains(input::getMouse().pos)) ||
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
						.rect = p->transformedRect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->transformedRect.tl + border, p->transformedRect.br - border),
						.color = p->backColor
					}, p->z),
					draw::make<draw::str>({
						.rect = jaw::recti(p->transformedRect.tl + border, p->transformedRect.br - border),
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
			.rect = &ep->transformedRect,
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

		ep->transformedRect = cam::forward(ep->camera, ep->rect);

		return x;
	}

	id createCheckbox(const UIElement &e) {
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::callbackid cbid = callback::create(jaw::callback{
			.data = (void*)(uintptr_t)x,
			.frameInterval = 1,
			.callback = [](jaw::callbackid cbid, jaw::properties *props) {
				jaw::callback *cb = callback::idtoptr(cbid);
				if (!cb) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)cb->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->transformedRect = cam::forward(p->camera, p->rect);

				constexpr int16_t BORDER = JAWUI_CHECKBOX_BORDER_WIDTH;
				constexpr int16_t STROKE = JAWUI_CHECKBOX_STROKE_WIDTH;
				draw::drawCall calls[4] = {
					draw::make<draw::rect>({
						.rect = p->transformedRect,
						.color = p->borderColor
					}, p->z),
					draw::make<draw::rect>({
						.rect = jaw::recti(p->transformedRect.tl + BORDER, p->transformedRect.br - BORDER),
						.color = p->backColor
					}, p->z),
					draw::make<draw::line>({
						.p1 = p->transformedRect.tl + 2*BORDER,
						.p2 = p->transformedRect.br - 2*BORDER,
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
			.rect = &ep->transformedRect,
			.condition = jaw::mouseFlags{.lmb = true },
			.data = (void*)(uintptr_t)x,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				if (p->selected) {
					p->selected = false;
					if (p->deselect) p->deselect(id, props);
				}
				else {
					p->selected = true;
					if (p->select) p->select(id, props);
				}
			}
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			callback::destroy(cbid);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		ep->transformedRect = cam::forward(ep->camera, ep->rect);

		return x;
	}

	namespace layout {
		Context *begin(const Context &c) {
			Context *x = util::frameAllocator->alloc<Context>(1);
			*x = {
				.type = c.type,
				.rect = c.rect,
				.gap = c.gap
			};
			return x;
		}

		static int16_t availableWidth(Context *ctx) {
			int16_t ret = ctx->rect.width();
			if (ctx->type == Type::ROW) {
				ret -= ctx->progress;
			}
			return ret;
		}
		static int16_t availableHeight(Context *ctx) {
			int16_t ret = ctx->rect.height();
			if (ctx->type == Type::COL) {
				ret -= ctx->progress;
			}
			return ret;
		}

		jaw::recti get(Context *ctx, const Options &opt) {
			const jaw::recti error { -1, -1, -1, -1 };

			// Handle either absolute or relative sizing
			// relative size is used if absoluteSize is 0,0
			jaw::recti ret;
			if (opt.absoluteSize == jaw::vec2i()) {
				ret = { 0, 0,
					(int16_t)(ctx->rect.width() * opt.relativeSize.x),
					(int16_t)(ctx->rect.height() * opt.relativeSize.y)
				};
			}
			else {
				ret = { { 0, 0 }, opt.absoluteSize };
			}

			// Handle errors for out-of-space
			if (ret.br.x > availableWidth(ctx) ||
				ret.br.y > availableHeight(ctx))
			{
				return error;
			}

			// Handle autofill
			if (ret.br.x <= 0) ret.br.x = availableWidth(ctx);
			if (ret.br.y <= 0) ret.br.y = availableHeight(ctx);

			// Move rect to be in the parent
			ret = ret + ctx->rect.tl;
			switch (ctx->type) {
			case Type::ROW:
				ret = ret + jaw::vec2i(ctx->progress, 0);
				ctx->progress += ret.width();
				ret.tl.x += ctx->gap/2;
				ret.br.x -= ctx->gap/2;
				break;
			case Type::COL:
				ret = ret + jaw::vec2i(0, ctx->progress);
				ctx->progress += ret.height();
				ret.tl.y += ctx->gap/2;
				ret.br.y -= ctx->gap/2;
				break;
			}

			return ret;
		}

		jaw::recti relrect(jaw::vec2i screenSize, jaw::vec2f reltl, jaw::vec2f reldim) {
			auto tl = jaw::vec2i(jaw::vec2f(screenSize) * reltl);
			auto br = tl + jaw::vec2i(jaw::vec2f(screenSize) * reldim);
			return jaw::recti(tl, br);
		}

		jaw::recti relsqr(jaw::vec2i screenSize, jaw::vec2f reltl, float reldim) {
			auto tl = jaw::vec2i(jaw::vec2f(screenSize) * reltl);
			float dim = std::min(screenSize.x, screenSize.y) * reldim;
			auto br = tl + jaw::vec2i(jaw::vec2f(dim, dim));
			return jaw::recti(tl, br);
		}
	}
}