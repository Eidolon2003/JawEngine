#include "../jawengine/JawEngine.h"
#include <string>

static jaw::properties props;

void game::init() {
	draw::setBackgroundColor(draw::color::RED);
}

void game::loop() {
	static std::string str;
	str = std::to_string(props.logicFrametime) + '\n' 
		+ std::to_string(props.totalFrametime) + '\n'
		+ std::to_string(props.framerate()) + '\n'
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
			2
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
}

int main() { 
	props.targetFramerate = 100.f;
	props.size = jaw::vec2i(300,200);
	props.scale = 4;
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}