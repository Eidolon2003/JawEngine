#include "JawEngine.h"

jaw::Sprite::Sprite() {
	x = y = dx = dy = 0;
	frame = 0;
	layer = 1;
	bmp = nullptr;
	src = Rect();
	scale = 1.f;
	hidden = false;
	lifetime = animationTiming = animationCounter = std::chrono::milliseconds(0);
}

jaw::Point jaw::Sprite::getPoint() const {
	return Point((int16_t)round(x), (int16_t)round(y));
}

void jaw::Sprite::setPoint(jaw::Point pos) {
	x = (float)pos.x; y = (float)pos.y;
}

jaw::Point jaw::Sprite::getSize() const {
	Point a;
	a.x = (int16_t)((src.br.x - src.tl.x) * scale);
	a.y = (int16_t)((src.br.y - src.tl.y) * scale);
	return a;
}

bool jaw::Sprite::Update(jaw::AppInterface* app) {
	auto frametime = app->window->getFrametime();

	if (lifetime.count() != 0) {
		if (lifetime <= frametime)
			return true;
		else
			lifetime -= frametime;
	}

	if (animationTiming.count() != 0) {
		animationCounter += frametime;
		if (animationCounter >= animationTiming) {
			animationCounter = std::chrono::milliseconds(0);

			uint16_t width = src.br.x - src.tl.x;
			uint16_t numFrames = bmp->getSize().x / width;
			frame++;
			if (frame >= numFrames) frame = 0;
		}
	}

	x += dx * (float)frametime.count() / 100;
	y += dy * (float)frametime.count() / 100;

	return false;
}

void jaw::Sprite::Draw(jaw::AppInterface* app) {
	app->graphics->DrawSprite(this);
}