#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"

#include <cstring>

// Demonstrate drawing a string from tempalloc memory
static void loop(jaw::properties *props) {
	char *str = util::tempalloc<char>(50);
	strncpy(str, "Hello, world!", 50);

	draw::enqueue(draw::str{
		.rect = jaw::recti(0, 0, 640, 480),
		.str = str,
		.color = jaw::color::WHITE
	}, 0);
}

int main() {
	jaw::properties props;
	engine::start(&props, nullptr, nullptr, loop);
}
