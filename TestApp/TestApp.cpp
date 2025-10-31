#include "../jawengine/JawEngine.h"
#include <iostream>

static void init(jaw::properties *props) {
	jaw::vec2i bmpSize;
	jaw::argb *px = asset::bmp("F:/assets/test-animation/walk-40x70x12.png", &bmpSize);
	if (!px) return;

	jaw::bmpid bmp = draw::createBmp(bmpSize);
	if (bmp == jaw::INVALID_ID) return;
	draw::writeBmp(bmp, px, bmpSize);

	jaw::animdefid animation = anim::create(
		jaw::animation{
			.endFrame = 11,
			.frameInterval = jaw::millis(70)
		}
	);

	jaw::sprid sprite = sprite::create(
		jaw::sprite{
			.bmp = bmp,
			.frameSize = jaw::vec2i(40, 70),
			.animState = anim::instanceOf(animation)
		}
	);
}

static void loop(jaw::properties *props) {}

int main() {
	std::vector<asset::INIEntry> vec;
	vec.emplace_back("width", "640", "Window width in pixels");
	vec.emplace_back("height", "480", "Window height in pixels");
	asset::ini("F:/assets/ini/test.ini", &vec);

	jaw::properties props;
	props.size = jaw::vec2i(
		std::stoi(vec[0].value) / 4,
		std::stoi(vec[1].value) / 4
	);
	props.scale = 4;

	engine::start(&props, nullptr, init, loop);
}