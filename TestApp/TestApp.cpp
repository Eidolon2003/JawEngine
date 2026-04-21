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

#include <iostream>

static jaw::recti rect;
static jaw::clickableid id;

void init(jaw::properties *p) {
	rect = jaw::recti(0, 0, 80, 80);
	id = input::createClickable(jaw::clickable{
		.rect = &rect,
		.callback = [](jaw::clickableid, jaw::properties*) { std::cout << "CLICK"; },
		.condition = {.lmb = true},
		.data = nullptr
	});
}

void loop(jaw::properties *p) {

}

int main() {
	jaw::properties props;
	props.targetFramerate = 1000;
	props.scale = 4;
	props.size.x = 160;
	props.size.y = 120;
	engine::start(&props, nullptr, init, loop);
}
