#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"
#include <string>

struct data {
	uint64_t lastFramecount = 0;
	jaw::nanoseconds sumFrametimes = 0;
	float framerate = 0;
	char inputString[256] = {};
	jaw::bmpid bmp = 0;
	jaw::vec2i bmpDim = 0;
};

static void init(jaw::properties* props) {
	data* d = (data*)props->data;

	input::clearAllBindings();
	input::bindKeyDown(key::ESC, [](jaw::properties*) { engine::stop(); });

	draw::setBackgroundColor(jaw::color::RED);

	jaw::argb* pixels;
	d->bmpDim = asset::bmp("F:/C++/GameJam/assets.png", &pixels);
	assert(pixels != nullptr);
	d->bmp = draw::createBmp(d->bmpDim);
	draw::writeBmp(d->bmp, pixels, d->bmpDim);
}

static void loop(jaw::properties* props) {
	data* d = (data*)props->data;

	// Compute the average framerate over the last 100 frames
	d->sumFrametimes += props->totalFrametime;
	if (props->framecount - d->lastFramecount == 100) {
		d->framerate = 100.f * 1'000'000'000.f / d->sumFrametimes;
		d->lastFramecount = props->framecount;
		d->sumFrametimes = 0;
	}

	const jaw::mouse* mouse = input::getMouse();
	input::getString(d->inputString, 256);

	static std::string str;
	str = std::to_string(props->logicFrametime) + '\n'
		+ std::to_string(props->totalFrametime) + '\n'
		+ std::to_string(d->framerate) + '\n'
		+ std::to_string(props->uptime / 1'000'000'000.f) + '\n'
		+ std::to_string(mouse->pos.x) + ',' + std::to_string(mouse->pos.y) + '\n'
		+ d->inputString;

	draw::str(
		draw::strOptions{
			jaw::recti(0,0,props->size.x, props->size.y),
			str.c_str(),
			jaw::color::WHITE,
			0,
			0.f
		},
		2
	);

	jaw::vec2i base(100, 0);
	draw::bmp(
		draw::bmpOptions{
			d->bmp,
			jaw::recti(jaw::vec2i(), d->bmpDim),
			jaw::recti(base, base + d->bmpDim),
			props->uptime / 1000000000.f
		},
		1
	);

	draw::rect(
		draw::rectOptions{
			jaw::recti(220, 100, 270, 150),
			jaw::color::GREEN,
			PI32/4
		},
		0
	);
}

int main() {
	data* d = new data;
	jaw::properties props;
	props.data = d;
	props.targetFramerate = 0;
	props.size = jaw::vec2i(300,200);
	props.scale = 4;
	props.mode = jaw::properties::WINDOWED;
	props.monitorIndex = -1;
	engine::start(&props, nullptr, init, loop);
}