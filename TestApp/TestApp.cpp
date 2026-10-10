/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#include "../jawengine/JawEngine.h"
#include "../jawengine/libs/ui.h"
#include <iostream>

void initOnce(jaw::properties *props) {
	(void)ui::createTextButton(ui::UIElement{
		.rect = jaw::recti(30, 30, 200, 200),
		.text = "Click Me!",
		.select = [](ui::id, jaw::properties*) { puts("CLICK"); }
	});
}

int main() {
	jaw::properties p { .showCMD = true };
	engine::start(&p, { .initOnce = initOnce });
}