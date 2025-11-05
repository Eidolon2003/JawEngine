#include "../jawengine/JawEngine.h"
#include <iostream>

static void loop(jaw::properties *props) {
	jaw::vec2i center(props->size.x / 2, props->size.y / 2);
	int16_t scale = std::min(center.x, center.y);

	auto *ctrl = input::getGamepad(0);
	if (!ctrl) {
		input::findNewGamepads();
		return;
	}

	jaw::vec2f stick = ctrl->sony.l;
	jaw::vec2i offset = center + jaw::vec2i(stick * scale);

	draw::enqueue(
		draw::rect{
			.rect = jaw::recti(offset-1, offset+2),
			.color = jaw::color::WHITE
		}, 0
	);

	char *buf = util::tempalloc<char>(128);
	snprintf(buf, 128, "%f\n%f", ctrl->sony.l2a, ctrl->sony.r2a);
	draw::enqueue(
		draw::str{
			.rect = jaw::recti(0, props->size),
			.str = buf,
			.color = jaw::color::WHITE
		}, 1
	);
	
	if (ctrl->sony.x.isDown) engine::stop();

	if (ctrl->sony.triangle.isHeld) std::cout << "Triangle Held\n";
	if (ctrl->sony.triangle.isDown) std::cout << "Triangle Down\n";

	if (ctrl->sony.up.isHeld) std::cout << "up\n";
	if (ctrl->sony.down.isHeld) std::cout << "down\n";
	if (ctrl->sony.left.isHeld) std::cout << "left\n";
	if (ctrl->sony.right.isHeld) std::cout << "right\n";

	auto a = input::getKey(key::A);
	if (a.isHeld) std::cout << "A Held\n";
	if (a.isDown) std::cout << "A Down\n";
}

int main() {
	std::vector<asset::INIEntry> vec;
	vec.emplace_back("width", "640", "Window width in pixels");
	vec.emplace_back("height", "480", "Window height in pixels");
	asset::ini("F:/assets/ini/test.ini", &vec);

	jaw::properties props;
	props.size = jaw::vec2i(
		std::stoi(vec[0].value),
		std::stoi(vec[1].value)
	);

	props.targetFramerate = 4;
	engine::start(&props, nullptr, nullptr, loop);
}