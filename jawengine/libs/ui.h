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
	// Computes the coordinates of a rect relative to the screen size
	inline jaw::recti relrect(jaw::vec2i screenSize, jaw::vec2f reltl, jaw::vec2f reldim) {
		auto tl = jaw::vec2i(jaw::vec2f(screenSize) * reltl);
		auto br = tl + jaw::vec2i(jaw::vec2f(screenSize) * reldim);
		return jaw::recti(tl, br);
	}

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
		jaw::sprid spr = jaw::INVALID_ID;
		jaw::clickableid click = jaw::INVALID_ID;
		ui::uifn select = nullptr, deselect = nullptr;
		bool selected = false;
	};
	
	inline util::slotAllocator<id, UIElement, MAX_NUM> slots;

	inline void clear() {
		slots.clear();
		sprite::clear();
		input::clear();
	}

	// Destroy and clean up any UI Element
	inline void destroy(ui::id x) {
		UIElement *e = slots.idtoptr(x);
		if (!e) [[unlikely]] return;
		if (e->click != jaw::INVALID_ID) input::destroy(e->click);
		if (e->spr != jaw::INVALID_ID) sprite::destroy(e->spr);
		slots.destroy(x);
	}

	inline UIElement *idtoptr(ui::id i) {
		return slots.idtoptr(i);
	}

	inline id createTextDisplay(const UIElement &e, uint8_t z)
	{
		constexpr int16_t BORDER = JAWUI_TEXT_DISPLAY_BORDER_WIDTH;

		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;

		jaw::sprid spr = sprite::create(jaw::sprite{
			.z = z,
			.data = (void*)(uintptr_t)x
		});
		if (spr == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		slots.idtoptr(x)->spr = spr;

		sprite::customDraw(spr, [](jaw::sprid sprid, jaw::properties*) {
			jaw::sprite *spr = sprite::idtoptr(sprid);
			if (!spr) [[unlikely]] return;

			ui::id id = (ui::id)(uintptr_t)spr->data;
			UIElement *p = slots.idtoptr(id);
			if (!p) [[unlikely]] return;

			draw::drawCall calls[3]{
				draw::make<draw::rect>({
					.rect = p->rect,
					.color = p->borderColor
				}, spr->z),
				draw::make<draw::rect>({
					.rect = jaw::recti(p->rect.tl + BORDER, p->rect.br - BORDER),
					.color = p->backColor
				}, spr->z),
				draw::make<draw::str>({
					.rect = jaw::recti(p->rect.tl + BORDER, p->rect.br - BORDER),
					.str = p->text,
					.color = p->textColor,
					.font = p->font
				}, spr->z)
			};
			draw::enqueueMany(calls, 3);
		});
		sprite::customUpdate(spr, [](jaw::sprid, jaw::properties*) {});

		return x;
	}

	inline id createTextButton(const UIElement &e, uint8_t z)
	{
		constexpr int16_t DESELECT_BORDER = JAWUI_TEXT_BUTTON_DESELECT_BORDER_WIDTH;
		constexpr int16_t SELECT_BORDER = JAWUI_TEXT_BUTTON_SELECT_BORDER_WIDTH;

		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::sprid spr = sprite::create(jaw::sprite{
			.z = z,
			.data = (void*)(uintptr_t)x
		});
		if (spr == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->spr = spr;

		jaw::clickableid click = input::createClickable(jaw::clickable{
			.rect = &ep->rect,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				if (p->select) p->select(id, props);
			},
			.condition = jaw::mouseFlags{ .lmb=true },
			.data = (void*)(uintptr_t)x
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			sprite::destroy(spr);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		sprite::customDraw(spr, [](jaw::sprid sprid, jaw::properties *props) {
			jaw::sprite *spr = sprite::idtoptr(sprid);
			if (!spr) [[unlikely]] return;

			ui::id id = (ui::id)(uintptr_t)spr->data;
			UIElement *p = slots.idtoptr(id);
			if (!p) [[unlikely]] return;

			int16_t border = p->rect.contains(props->mouse.pos) ? SELECT_BORDER : DESELECT_BORDER;

			draw::drawCall calls[3]{
				draw::make<draw::rect>({
					.rect = p->rect,
					.color = p->borderColor
				}, spr->z),
				draw::make<draw::rect>({
					.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
					.color = p->backColor
				}, spr->z),
				draw::make<draw::str>({
					.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
					.str = p->text,
					.color = p->textColor,
					.font = p->font
				}, spr->z)
			};
			draw::enqueueMany(calls, 3);
		});
		sprite::customUpdate(spr, [](jaw::sprid, jaw::properties*) {});

		return x;
	}

	inline id createTextInput(const UIElement &e, uint8_t z) {
		constexpr int16_t DESELECT_BORDER = JAWUI_TEXT_INPUT_DESELECT_BORDER_WIDTH;
		constexpr int16_t SELECT_BORDER = JAWUI_TEXT_INPUT_SELECT_BORDER_WIDTH;

		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::sprid spr = sprite::create(jaw::sprite{
			.z = z,
			.data = (void*)(uintptr_t)x
		});
		if (spr == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->spr = spr;

		jaw::clickableid click = input::createClickable(jaw::clickable{
			.rect = &ep->rect,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->selected = true;
				if (p->select) p->select(id, props);
			},
			.condition = jaw::mouseFlags{.lmb = true },
			.data = (void*)(uintptr_t)x
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			sprite::destroy(spr);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		sprite::customDraw(spr, [](jaw::sprid sprid, jaw::properties *props) {
			jaw::sprite *spr = sprite::idtoptr(sprid);
			if (!spr) [[unlikely]] return;

			ui::id id = (ui::id)(uintptr_t)spr->data;
			UIElement *p = slots.idtoptr(id);
			if (!p) [[unlikely]] return;

			int16_t border = p->selected ? SELECT_BORDER : DESELECT_BORDER;

			draw::drawCall calls[3]{
				draw::make<draw::rect>({
					.rect = p->rect,
					.color = p->borderColor
				}, spr->z),
				draw::make<draw::rect>({
					.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
					.color = p->backColor
				}, spr->z),
				draw::make<draw::str>({
					.rect = jaw::recti(p->rect.tl + border, p->rect.br - border),
					.str = p->text,
					.color = p->textColor,
					.font = p->font
				}, spr->z)
			};
			draw::enqueueMany(calls, 3);
		});

		sprite::customUpdate(spr, [](jaw::sprid sprid, jaw::properties *props) {
			jaw::sprite *spr = sprite::idtoptr(sprid);
			if (!spr) [[unlikely]] return;

			ui::id id = (ui::id)(uintptr_t)spr->data;
			UIElement *p = slots.idtoptr(id);
			if (!p) [[unlikely]] return;

			if (p->selected && (
					(props->mouse.flags.lmb && !p->rect.contains(props->mouse.pos)) ||
					(input::getKey(key::ENTER).isDown && !input::getKey(key::SHIFT).isHeld)
				)
			) {
				p->selected = false;
				if (p->deselect) p->deselect(id, props);
			}
			
			if (p->selected) input::getString(p->text, JAWUI_TEXT_CAPACITY);
		});

		return x;
	}

	inline id createCheckbox(const UIElement &e, uint8_t z) {
		constexpr int16_t BORDER = JAWUI_CHECKBOX_BORDER_WIDTH;
		constexpr int16_t STROKE = JAWUI_CHECKBOX_STROKE_WIDTH;

		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) [[unlikely]] return jaw::INVALID_ID;
		UIElement *ep = slots.idtoptr(x);

		jaw::sprid spr = sprite::create(jaw::sprite{
			.z = z,
			.data = (void*)(uintptr_t)x
			});
		if (spr == jaw::INVALID_ID) [[unlikely]] {
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->spr = spr;

		jaw::clickableid click = input::createClickable(jaw::clickable{
			.rect = &ep->rect,
			.callback = [](jaw::clickableid cid, jaw::properties *props) {
				jaw::clickable *click = input::idtoptr(cid);
				if (!click) [[unlikely]] return;

				ui::id id = (ui::id)(uintptr_t)click->data;
				UIElement *p = slots.idtoptr(id);
				if (!p) [[unlikely]] return;

				p->selected = !p->selected;
			},
			.condition = jaw::mouseFlags{.lmb = true },
			.data = (void*)(uintptr_t)x
		});
		if (click == jaw::INVALID_ID) [[unlikely]] {
			sprite::destroy(spr);
			slots.destroy(x);
			return jaw::INVALID_ID;
		}
		ep->click = click;

		sprite::customDraw(spr, [](jaw::sprid sprid, jaw::properties *props) {
			jaw::sprite *spr = sprite::idtoptr(sprid);
			if (!spr) [[unlikely]] return;

			ui::id id = (ui::id)(uintptr_t)spr->data;
			UIElement *p = slots.idtoptr(id);
			if (!p) [[unlikely]] return;

			draw::drawCall calls[4] = {
				draw::make<draw::rect>({
					.rect = p->rect,
					.color = p->borderColor
				}, spr->z),
				draw::make<draw::rect>({
					.rect = jaw::recti(p->rect.tl + BORDER, p->rect.br - BORDER),
					.color = p->backColor
				}, spr->z),
				draw::make<draw::line>({
					.p1 = p->rect.tl + 2*BORDER,
					.p2 = p->rect.br - 2*BORDER,
					.color = p->textColor,
					.width = STROKE
				}, spr->z)
			};
			draw::enqueueMany(calls, 2 + p->selected);
		});
	}
}