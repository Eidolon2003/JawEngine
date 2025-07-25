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
}

int main() { 
	props.targetFramerate = 100.f;
	props.size = jaw::vec2i(200,150);
	props.scale = 8;
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}