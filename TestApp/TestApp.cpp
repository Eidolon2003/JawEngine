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
	jaw::sprid spr1 = 0;
	jaw::sprid spr2 = 0;
};

static void init(jaw::properties* props) {
	data* d = (data*)props->data;

	input::clearAllBindings();
	input::bindKeyDown(key::ESC, [](jaw::properties*) { engine::stop(); });

	draw::setBackgroundColor(jaw::color::RED);

	jaw::argb* pixels;
	d->bmpDim = asset::bmp("F:/C++/GameJam/assets.png", &pixels);
	if (pixels) {
		d->bmp = draw::createBmp(d->bmpDim);
		if (d->bmp != jaw::INVALID_ID) {
			draw::writeBmp(d->bmp, pixels, d->bmpDim);
		}
	}

	sprite::clear();
	d->spr1 = sprite::create(
		{
			.pos = jaw::vec2f(0, 0),
			.vel = jaw::vec2f(20, 0),
			.z = 1,
			.bmp = d->bmp,
			.frameSize = d->bmpDim
		}
	);

	auto anim = anim::create(
		{
			.startFrame = 0,
			.endFrame = 3,
			.row = 2,
			.frameInterval = jaw::seconds(0.2f),
			.loop = true
		}
	);

	d->spr2 = sprite::create(
		{
			.pos = jaw::vec2f(20, 180),
			.bmp = d->bmp,
			.frameSize = jaw::vec2i(16, 16),
			.animState = anim::instanceOf(anim)
		}
	);
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
		+ std::to_string(jaw::to_seconds(props->uptime)) + '\n'
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