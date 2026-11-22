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

// Sprite demo

#include <JawEngine.h>
#include <libs/profile.h>
#include <iostream>

static void loop(jaw::properties *props) {
	profile::begin(0);
	util::accurateSleep(jaw::millis(420.69), profile::get(0));
	profile::end(0);
	std::cout << jaw::to_millis(profile::get(0)) << " ms\n";
}

int main() {
	jaw::properties p;
	p.showCMD = true;
	engine::start(&p, { .loop = loop });
}

#include <libs/profile.cpp>