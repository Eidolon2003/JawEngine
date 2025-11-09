#include "../jawengine/JawEngine.h"

static void loop(jaw::properties *props) {
	jaw::vec2i m = props->mouse.pos;
	const jaw::recti rect(100, 100, 500, 500);
	draw::enqueue(draw::rect{
		.rect = rect,
		.color = rect.contains(m) ? jaw::color::GREEN : jaw::color::RED
	}, 0);

	jaw::recti mouseRect(m-19, m+20);
	draw::enqueue(draw::rect{
		.rect = mouseRect,
		.color = mouseRect.collides(rect) ? jaw::color::CYAN : jaw::color::MAGENTA
	}, 1);
}

int main() {
	jaw::properties props;
	props.size = jaw::vec2i(1280,1024);
	engine::start(&props, nullptr, nullptr, loop);
}