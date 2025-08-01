#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"
#include <string>

static jaw::properties props;
static uint64_t lastFramecount;
static jaw::nanoseconds sumFrametimes;
static float framerate;
static char inputString[256];

static jaw::bmpid bmp;

void game::init() {
	draw::setBackgroundColor(jaw::color::RED);

	char* p1, * p2;
	size_t s1 = asset::file("F:/C++/GameJam/assets.png", (void**)&p1);
	assert(p1 != nullptr);
	size_t s2 = asset::file("F:/C++/GameJam/assets.png", (void**)&p2);
	assert(p1 == p2 && s1 == s2);

	strncat(inputString, p1 + 1, 3);	//Writes "PNG" to the screen


	bmp = draw::createRenderableBmp(jaw::vec2i(64, 64));

	auto opt = draw::rectOptions{
		jaw::recti(10,10,50,50),
		jaw::color::WHITE
	};

	draw::renderToBmp(
		draw::makeDraw(
			draw::RECT,
			0,
			&opt
		),
		bmp
	);
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
			bmp,
			jaw::recti(0,0,64,64),
			jaw::recti(100,100,164,164)
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