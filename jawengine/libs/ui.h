#pragma once
#include "../JawEngine.h"
#include <string>
#include <bit>

namespace ui {
	constexpr size_t MAX_NUM = 128;

	struct UIElement {
		jaw::recti rect;
		const char *text;
		jaw::fontid font;
		jaw::argb backColor;
		jaw::argb borderColor;
		jaw::argb textColor;
		jaw::sprid bmp;
		jaw::clickfn select, deselect;
		bool selected;
	};
	typedef uint32_t id;

	inline util::slotAllocator<id, UIElement, MAX_NUM> slots;

	inline id makeTextDisplayBox(jaw::recti rect, const char *str, jaw::fontid font,
		uint8_t z, jaw::argb backColor, jaw::argb borderColor, jaw::argb textColor)
	{
		constexpr int16_t BORDER_WIDTH = 2;

		UIElement e = {
			.rect = rect,
			.text = str,
			.font = font,
			.backColor = backColor,
			.borderColor = borderColor,
			.textColor = textColor
		};
		ui::id x = slots.create(&e);
		if (x == jaw::INVALID_ID) return jaw::INVALID_ID;

		jaw::sprid spr = sprite::create(jaw::sprite{
			.z = z,
			.data = (void*)(uintptr_t)x
		});
		if (spr == jaw::INVALID_ID) return jaw::INVALID_ID;

		sprite::customDraw(spr, [](jaw::sprid sprid, jaw::properties*) {
			jaw::sprite *spr = sprite::idtoptr(sprid);
			if (!spr) return;

			ui::id id = (ui::id)(uintptr_t)spr->data;
			UIElement *p = slots.idtoptr(id);
			if (!p) return;

			draw::enqueue(draw::rect{
				.rect = p->rect,
				.color = p->borderColor
			}, spr->z);
			draw::enqueue(draw::rect{
				.rect = jaw::recti(p->rect.tl + BORDER_WIDTH, p->rect.br - BORDER_WIDTH),
				.color = p->backColor
			}, spr->z);
			draw::enqueue(draw::str{
				.rect = jaw::recti(p->rect.tl + BORDER_WIDTH, p->rect.br - BORDER_WIDTH),
				.str = p->text,
				.color = p->textColor,
				.font = p->font
			}, spr->z);
		});
		sprite::customUpdate(spr, [](jaw::sprid, jaw::properties*) {});

		return x;
	}
}