/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

// Demonstrate drawing primitives

#include <JawEngine.h>
#include <cmath>

static void loop(jaw::properties *props) {
	// Solid rectangle
	draw::enqueue<draw::rect>({
		.rect = jaw::recti(50, 50, 150, 150),
		.color = jaw::color::RED
	}, 0);

	// Hollow diamond
	draw::enqueue<draw::rect>({
		.rect = jaw::recti(200, 200, 300, 300),
		.color = jaw::color::GREEN,
		.width = 10,
		.angle = PI32/4
	}, 0);

	// Circle
	draw::enqueue<draw::ellipse>({
		.ellipse = jaw::ellipse(jaw::vec2i(400, 400), jaw::vec2i(100, 100)),
		.color = jaw::color::RED
	}, 0);

	// Translucent blue ellipse on top of circle
	draw::enqueue<draw::ellipse>({
		.ellipse = jaw::ellipse(jaw::vec2i(400, 400), jaw::vec2i(150, 80)),
		.color = 0x880000FF
	}, 1);

	// Draw Clock
	float uptimeInSeconds = jaw::to_seconds(props->uptime);
	float time = uptimeInSeconds * 1000; // run the clock at 1000x speed
	float minute = fmodf(time / 60, 60);
	float hour = fmodf(time / 3600, 12);

	float minuteAngle = PI32/2 - 2*PI32*(minute/60);
	auto minuteVec = jaw::vec2f(cosf(minuteAngle), -sinf(minuteAngle));

	float hourAngle = PI32/2 - 2*PI32*(hour/12);
	auto hourVec = jaw::vec2f(cosf(hourAngle), -sinf(hourAngle));

	draw::enqueue<draw::ellipse>({
		.ellipse = jaw::ellipse(jaw::vec2i(650, 150), jaw::vec2i(100, 100)),
		.color = jaw::color::WHITE,
		.width = 5
	}, 1);
	draw::enqueue<draw::line>({
		.p1 = jaw::vec2i(650, 150),
		.p2 = jaw::vec2i(650, 150) + jaw::vec2i(minuteVec * 90),
		.color = jaw::color::WHITE,
		.width = 5
	}, 2);
	draw::enqueue<draw::line>({
		.p1 = jaw::vec2i(650, 150),
		.p2 = jaw::vec2i(650, 150) + jaw::vec2i(hourVec * 70),
		.color = jaw::color::WHITE,
		.width = 5
	}, 2);
}

int main() {
	jaw::properties p {
		.size = {800, 600},
	};

	engine::start(&p, { .loop = loop });
}
