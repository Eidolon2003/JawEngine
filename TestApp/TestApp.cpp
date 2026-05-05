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
#include "../jawengine/libs/ui.h"

static ui::id box;
static float a;

static void init(jaw::properties *p) {
	box = ui::createCheckbox(ui::UIElement{
		.rect = jaw::recti(10,10,30,30),
		.borderColor = jaw::color::RED,
		.textColor = jaw::color::GREEN
	}, 0);
}

static void loop(jaw::properties *p) {
	if (input::getKey(key::Q).isHeld) a -= jaw::to_seconds(p->totalFrametime);
	else if (input::getKey(key::E).isHeld) a += jaw::to_seconds(p->totalFrametime);

	draw::enqueue(draw::line{
		.p1 = jaw::vec2i(50, 50),
		.p2 = jaw::vec2i(100, 100),
		.color = jaw::color::WHITE,
		.width = 2,
		.angle = a
	}, 0);
}

int main() {
	jaw::properties props;
	props.targetFramerate = 500;
	props.scale = 3;
	props.size.x = 240;
	props.size.y = 180;
	props.title = "UI Demo";
	engine::start(&props, nullptr, init, loop); 
}