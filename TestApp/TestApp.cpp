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

// Demo ui layout system

#include <JawEngine.h>
#include <libs/ui.cpp>

static void init(jaw::properties *props) {
	auto parentRect = ui::layout::relrect(
		props->size,
		jaw::vec2f(0.25f, 0.f),
		jaw::vec2f(0.5f, 1.f)
	);

	(void)ui::createTextDisplay({ .rect = parentRect });

	auto layout = ui::layout::begin({
		.type = ui::layout::Type::COL,
		.rect = parentRect,
		.gap = 10
	});
	(void)ui::createTextDisplay({
		.rect = ui::layout::get(layout, { .absoluteSize = { 0, 100 }}),
		.text = "A"
	});
	(void)ui::createTextDisplay({
		.rect = ui::layout::get(layout, { .absoluteSize = { 0, 100 }}),
		.text = "B"
	});
	(void)ui::createTextDisplay({
		.rect = ui::layout::get(layout, { .absoluteSize = { 0, 100 }}),
		.text = "C"
	});

	auto layoutRow = ui::layout::begin({
		.type = ui::layout::Type::ROW,
		.rect = ui::layout::get(layout, { .absoluteSize = { 0, 100 }}),
		.gap = 10
	});
	(void)ui::createTextDisplay({
		.rect = ui::layout::get(layoutRow, { .relativeSize = { 0.5f, 0 }}),
		.text = "D"
	});
	(void)ui::createTextDisplay({
		.rect = ui::layout::get(layoutRow, { .relativeSize = { 0.5f, 0 }}),
		.text = "E"
	});
}

int main() {
	jaw::properties p;
	engine::start(&p, { .init = init });
}