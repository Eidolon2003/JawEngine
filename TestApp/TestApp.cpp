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

ui::id box;

static void init(jaw::properties *p) {
	box = ui::makeTextDisplayBox(
		jaw::recti(10, 10, 100, 30),
		" HELLO",
		0,
		0,
		jaw::color::DARK_RED,
		jaw::color::BLUE,
		jaw::color::WHITE
	);
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