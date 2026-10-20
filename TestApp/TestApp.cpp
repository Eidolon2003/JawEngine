/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

// Demo UI elements with cameras
// The text box follows the mouse via a camera, while its logical position remains constant

#include <JawEngine.h>
#include <libs/ui.cpp>

static jaw::fontid font;
static cam::id camera;
static ui::id uiel;

static void init(jaw::properties *) {
	font = draw::newFont({
		.name = "Courier New",
		.size = 48.0f,
		.alignx = draw::font::CENTERX,
		.aligny = draw::font::CENTERY
	});

	camera = cam::create({
		.multiplier = 1.0f,
		.offset = {}
	});

	uiel = ui::createTextDisplay({
		.text = "Hello!",
		.font = font,
		.rect = jaw::recti(0, 0, 300, 80),
		.camera = camera
	});
}

static void loop(jaw::properties *) {
	auto mousePos = input::getMouse().pos;
	cam::Camera *c = cam::idtoptr(camera);
	if (c) c->offset = mousePos;
}

int main() {
	jaw::properties p;
	engine::start(&p, { .loop = loop, .init = init });
}