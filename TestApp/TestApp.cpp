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

// Verify rect collision logic

#include <JawEngine.h>
#include <libs/cam.cpp>
#include <iostream>

static jaw::recti rectA = jaw::recti(500, 100, 560, 360);
static jaw::recti rectB = jaw::recti(1000, 100, 1500, 600);
static jaw::recti rectC = jaw::recti(0, 0, 250, 50);

void loop(jaw::properties *) {
	// draw rectA and rectB in blue
	draw::enqueue<draw::rect>({
		.rect = rectA,
		.color = jaw::color::BLUE
	}, 0);
	draw::enqueue<draw::rect>({
		.rect = rectB,
		.color = jaw::color::BLUE
	}, 0);

	// rectC is relative to the mouse
	auto rectC_offset = rectC + input::getMouse().pos;

	bool isColliding = rectC_offset.collides(rectA) || rectC_offset.collides(rectB);

	// Draw rectC as red or green depending on collision
	draw::enqueue<draw::rect>({
		.rect = rectC_offset,
		.color = isColliding ? jaw::color::GREEN : jaw::color::RED
	}, 0);
}

int main() {
	jaw::properties p;
	p.size = { 1600,1200 };
	p.showCMD = true;
	engine::start(&p, { .loop = loop });
}