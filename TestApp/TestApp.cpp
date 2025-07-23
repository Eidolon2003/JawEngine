#include "../jawengine/JawEngine.h"
#include <iostream>

static jaw::properties props;

void game::init() {
	draw::setBackgroundColor(draw::color::RED);
}

void game::loop() {
	draw::str(
		draw::strOptions{
			jaw::recti(0,0,props.size.x, props.size.y),
			draw::color::WHITE,
			0,
			"TEST STRING FOR PIXEL CHECKING"
		},
		0
	);
}

int main() { 
	props.size = jaw::vec2i(640,480);
	props.scale = 0;
	props.mode = jaw::properties::FULLSCREEN_CENTERED_INTEGER;
	engine::start(&props);
}