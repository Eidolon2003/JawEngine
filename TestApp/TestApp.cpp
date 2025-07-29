#include "../jawengine/JawEngine.h"
#include <string>

static jaw::properties props;

static uint64_t lastFramecount;
static jaw::nanoseconds sumFrametimes;
static float framerate;
static draw::bmpid bitmap;

static draw::argb pixels[64 * 64];

void game::init() {
	draw::setBackgroundColor(draw::color::RED);
	bitmap = draw::createBmp(jaw::vec2i(64, 64));
}

void game::loop() {
	// Compute the average framerate over the last 100 frames
	sumFrametimes += props.totalFrametime;
	if (props.framecount - lastFramecount == 100) {
		framerate = 100.f * 1'000'000'000.f / sumFrametimes;
		lastFramecount = props.framecount;
		sumFrametimes = 0;
	}

	static std::string str;
	str = std::to_string(props.logicFrametime) + '\n' 
		+ std::to_string(props.totalFrametime) + '\n'
		+ std::to_string(framerate) + '\n'
		+ std::to_string(engine::getUptime() / 1'000'000'000.f);

	draw::str(
		draw::strOptions{
			jaw::recti(0,0,props.size.x, props.size.y),
			draw::color::WHITE,
			0,
			str.c_str()
		},
		0
	);

	draw::ellipse(
		draw::ellipseOptions{
			jaw::ellipse(
				jaw::vec2i(120,100),
				jaw::vec2i(30, 40)
			),
			draw::color::CYAN
		},
		2
	);

	draw::line(
		draw::lineOptions{
			jaw::vec2i(20, 100),
			jaw::vec2i(80, 100),
			draw::color::BLUE,
			1
		},
		1
	);

	draw::line(
		draw::lineOptions{
			jaw::vec2i(20, 105),
			jaw::vec2i(80, 105),
			draw::color::BLUE,
			2
		},
		1
	);

	draw::line(
		draw::lineOptions{
			jaw::vec2i(20, 110),
			jaw::vec2i(80, 110),
			draw::color::BLUE,
			3
		},
		1
	);

	draw::line(
		draw::lineOptions{
			jaw::vec2i(120, 20),
			jaw::vec2i(240, 180),
			draw::color::BLUE,
			2
		},
		1
	);

	draw::str(
		draw::strOptions{
			jaw::recti(50, 150, 300, 200),
			draw::color::WHITE,
			0,
			"SPHINX OF BLACK QUARTZ, JUDGE MY VOW"
		},
		3
	);

	draw::str(
		draw::strOptions{
			jaw::recti(25, 175, 300, 200),
			draw::color::WHITE,
			0,
			"the quick brown fox jumps over the lazy dog"
		},
		3
	);

	draw::rect(
		draw::rectOptions{
			jaw::recti(200, 10, 275, 100),
			draw::color::MAGENTA
		},
		0
	);

	pixels[5] = 0xFFFFFFFF;
	draw::writeBmp(bitmap, pixels, 64*64);
	draw::bmp(
		draw::bmpOptions{
			bitmap,
			jaw::recti(0,0,64,64),
			jaw::recti(20,80,84,144)
		},
		5
	);
}

int main() { 
	props.targetFramerate = 0;	//VSync ON
	props.size = jaw::vec2i(300,200);
	props.scale = 4;
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}