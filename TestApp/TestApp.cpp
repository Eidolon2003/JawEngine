#include "../jawengine/JawEngine.h"
#include <string>
#include <iostream>

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
}

void game::loop() {
	if (input::getKey(key::ESC).isDown) engine::stop();

	// Compute the average framerate over the last 10 frames
	sumFrametimes += props.totalFrametime;
	if (props.framecount - lastFramecount == 10) {
		framerate = 10.f * 1'000'000'000.f / sumFrametimes;
		lastFramecount = props.framecount;
		sumFrametimes = 0;
	}

	auto a = input::getKey(key::A);
	std::cout << a.isDown << ", " << a.isHeld << '\n';

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
		0
	);
}

int main() {
	props.targetFramerate = 5;
	props.size = jaw::vec2i(300,200);
	props.scale = 4;
	props.mode = jaw::properties::WINDOWED;
	props.showCMD = true;
	engine::start(&props);
}