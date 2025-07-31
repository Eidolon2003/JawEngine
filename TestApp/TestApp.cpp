#include "../jawengine/JawEngine.h"
#include <string>

static jaw::properties props;
static uint64_t lastFramecount;
static jaw::nanoseconds sumFrametimes;
static float framerate;
static jaw::bmpid bitmap;
static jaw::argb pixels[64 * 64];
static char inputString[256];

void game::init() {
	draw::setBackgroundColor(jaw::color::RED);

	bitmap = draw::createBmp(jaw::vec2i(64, 64));

	for (int i = 0; i < 64 * 64; i++) {
		pixels[i] = 0x88FFFFFF;
	}
	draw::writeBmp(bitmap, pixels, 64 * 64);
}

void game::loop() {
	if (input::getKey(key::ESC).isDown) engine::stop();

	// Compute the average framerate over the last 1000 frames
	sumFrametimes += props.totalFrametime;
	if (props.framecount - lastFramecount == 1000) {
		framerate = 1000.f * 1'000'000'000.f / sumFrametimes;
		lastFramecount = props.framecount;
		sumFrametimes = 0;
	}

	const jaw::mouse* mouse = input::getMouse();
	input::getString(inputString, 256);

	static std::string str;
	str = std::to_string(props.logicFrametime) + '\n'
		+ std::to_string(props.totalFrametime) + '\n'
		+ std::to_string(framerate) + '\n'
		+ std::to_string(engine::getUptime() / 1'000'000'000.f) + '\n'
		+ std::to_string(mouse->pos.x) + ',' + std::to_string(mouse->pos.y) + '\n'
		+ inputString;

	draw::str(
		draw::strOptions{
			jaw::recti(0,0,props.size.x, props.size.y),
			jaw::color::WHITE,
			0,
			str.c_str()
		},
		2
	);

	draw::bmp(
		draw::bmpOptions{
			bitmap,
			jaw::recti(0,0,64,64),
			jaw::recti(20, 100, 84, 164)
		},
		1
	);
}

int main() {
	props.targetFramerate = 10000;
	props.size = jaw::vec2i(300,200);
	props.scale = 4;
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}