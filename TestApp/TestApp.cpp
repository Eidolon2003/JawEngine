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

static void loop(jaw::properties *p) {
	draw::enqueue(draw::ellipse{
		.ellipse = jaw::ellipse(p->size/5, p->size/5),
		.color = jaw::color::WHITE,
		.width = 2
	}, 0);

	draw::enqueue(draw::rect{
		.rect = jaw::recti(p->size/2, p->size),
		.color = jaw::color::WHITE,
		.width = 3
	}, 0);
}

int main() {
	jaw::properties props;
	props.targetFramerate = 500;
	props.scale = 50;
	props.size.x = 20;
	props.size.y = 20;
	props.title = "Hollow Shapes";
	engine::start(&props, nullptr, nullptr, loop); 
}