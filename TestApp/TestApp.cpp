#include "../jawengine/JawEngine.h"
#include <iostream>

jaw::properties props;

static draw::drawCall pixelCalls[320 * 240];

void game::init() {
	srand((unsigned)time(NULL));

	draw::rectOptions opt{};
	for (int i = 0; i < 320; i++) {
		for (int j = 0; j < 240; j++) {
			size_t index = j * 320 + i;
			opt.rect = jaw::recti(i, j, i + 1, j + 1);
			pixelCalls[index] = draw::makeDraw(draw::type::RECT, 0, &opt);
		}
	}
}

draw::argb makeColor() {
	uint32_t a = 0xFF;
	uint32_t r = rand() & 0xFF;
	uint32_t g = rand() & 0xFF;
	uint32_t b = rand() & 0xFF;
	return (a << 24) | (r << 16) | (g << 8) | b;
}

void game::loop() {
	for (int i = 0; i < 320 * 240; i++) {
		((draw::rectOptions*)(pixelCalls[i].data))->color = makeColor();
	}
	draw::enqueueMany(pixelCalls, 320 * 240 / 2);

	static char fps[64];
	snprintf(fps, 64, "%d", (int)props.getFramerate());
	draw::strOptions opt{};
	opt.rect = jaw::recti(0, 0, 320, 240);
	opt.color = draw::color::WHITE;
	opt.font = 0;
	opt.str = fps;
	draw::str(&opt, 1);
}

int main() { 
	props.framerate = 0;
	props.size = jaw::vec2i(320, 240);
	props.scale = 6;
	props.title = "Test Title";
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}