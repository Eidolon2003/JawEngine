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

int main() {
	const char *filename = "F:\\assets\\ini\\test.ini";

	std::vector<asset::INIEntry> vec;
	vec.emplace_back(
		"x", "55",
		"X COMMENT!!"
	);
	vec.emplace_back(
		"y", "99",
		"Y COMMENT!!"
	);

	asset::readINI(filename, &vec);

	int x = std::stoi(vec[0].value);
	int y = std::stoi(vec[1].value);

	std::cout << "The current value of x is " << x << std::endl;
	std::cout << "New value? ";
	std::cin >> x;

	vec[0].value = std::to_string(x);

	asset::writeINI(filename, &vec);
}
