/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025 Julian Williams
 *
 * JawEngine 0.2.0
 * https://github.com/Eidolon2003/JawEngine
 */

#include "../jawengine/JawEngine.h"

#define JAWUI_TEXT_DISPLAY_BORDER_WIDTH 1
#include "../jawengine/libs/ui.h"

ui::id box;
ui::id in;

static void init(jaw::properties *p) {
	jaw::fontid font = draw::newFont(draw::font{
		.name = "Courier New",
		.size = 12.f,
		.alignx = draw::font::CENTERX,
		.aligny = draw::font::CENTERY
	});

	box = ui::createTextDisplay(ui::UIElement{
		.text = "HELLO",
		.rect = jaw::recti(10,10,100,30),
		.font = font,
	}, 0);

	in = ui::createTextInput(ui::UIElement{
		.text = "",
		.rect = jaw::recti(10, 50, 100, 70),
		.font = font,
		.select = [](ui::id, jaw::properties*) {
			ui::UIElement *a = ui::idtoptr(in);
			if (a == nullptr) return;
			else a->text[0] = 0;
		},
		.deselect = [](ui::id, jaw::properties*) {
			ui::UIElement *a = ui::idtoptr(box);
			ui::UIElement *b = ui::idtoptr(in);
			if (a == nullptr || b == nullptr) return;
			else strncpy(a->text, b->text, JAWUI_TEXT_CAPACITY);
		}
	}, 0);
}

static void loop(jaw::properties *p) {

}

int main() {
	jaw::properties props;
	props.targetFramerate = 1000;
	props.scale = 3;
	props.size.x = 240;
	props.size.y = 180;
	props.title = "UI Demo";
	engine::start(&props, nullptr, init, loop);
}