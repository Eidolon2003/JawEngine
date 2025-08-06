#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"
#include <string>

static jaw::properties props;
static uint64_t lastFramecount;
static jaw::nanoseconds sumFrametimes;
static float framerate;
static char inputString[256];

static jaw::bmpid bmp;
static jaw::vec2i bmpDim;

void game::init() {
	draw::setBackgroundColor(jaw::color::RED);

	// This is the new procedure for loading a bitmap
	// Split the asset and draw system apart, and give raw access to pixel data for any manipulation
	jaw::argb* pixels;
	bmpDim = asset::bmp("F:/C++/GameJam/assets.png", &pixels);
	assert(pixels != nullptr);
	bmp = draw::createBmp(bmpDim);
	draw::writeBmp(bmp, pixels, bmpDim);
}

void game::loop() {
	if (input::getKey(key::ESC).isDown) engine::stop();

	// Compute the average framerate over the last 100 frames
	sumFrametimes += props.totalFrametime;
	if (props.framecount - lastFramecount == 100) {
		framerate = 100.f * 1'000'000'000.f / sumFrametimes;
		lastFramecount = props.framecount;
		sumFrametimes = 0;
	}

	const jaw::mouse* mouse = input::getMouse();
	input::getString(inputString, 256);

	static std::string str;
	str = std::to_string(props.logicFrametime) + '\n'
		+ std::to_string(props.totalFrametime) + '\n'
		+ std::to_string(framerate) + '\n'
		+ std::to_string(props.uptime / 1'000'000'000.f) + '\n'
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

	jaw::vec2i base(100, 0);
	draw::bmp(
		draw::bmpOptions{
			bmp,
			jaw::recti(jaw::vec2i(), bmpDim),
			jaw::recti(base, base + bmpDim)
		},
		0
	);
}

int main() {
	props.targetFramerate = 0;
	props.size = jaw::vec2i(300,200);
	props.scale = 4;
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}