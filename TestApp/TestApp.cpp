#include "../jawengine/JawEngine.h"
#include <iostream>

static void init(jaw::properties *props) {
	size_t size = 1;
	unsigned char *buf = (unsigned char*)util::mapCircularBuffer(&size);
	buf[0] = 10;
	if (buf[size] == 10) {
		std::cout << "It works!";
	}
	else {
		std::cout << "It doesn't work...";
	}
	util::unmapCircularBuffer(buf, size);
}

static void loop(jaw::properties *props) {}

int main() {
	std::vector<asset::INIEntry> vec;
	vec.emplace_back("width", "640", "Window width in pixels");
	vec.emplace_back("height", "480", "Window height in pixels");
	asset::ini("F:/assets/ini/test.ini", &vec);

	jaw::properties props;
	props.size = jaw::vec2i(
		std::stoi(vec[0].value),
		std::stoi(vec[1].value)
	);

	props.showCMD = true;
	engine::start(&props, nullptr, init, loop);
}