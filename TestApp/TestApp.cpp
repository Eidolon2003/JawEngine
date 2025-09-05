#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"
#include <cstdio>

static jaw::animdefid animation;
static jaw::bmpid bitmap;
static jaw::sprid maxID;
static char buf[256];

void timedDestroy(jaw::sprid spr, jaw::properties* props) {
	sprite::update(spr, props);
	auto ptr = sprite::idtoptr(spr);
	if (ptr->age > jaw::seconds(5)) {
		anim::destroy(ptr->animState);
		sprite::destroy(spr);
	}
}

void createSprite(jaw::properties* props) {
	auto anim = anim::instanceOf(animation);
	auto id = sprite::create(jaw::sprite{
		.pos = props->mouse.pos.tofloat(),
		.vel = jaw::vec2f(0, -25),
		.bmp = bitmap,
		.frameSize = jaw::vec2i(16, 16),
		.animState = anim
	});

	sprite::customUpdate(id, timedDestroy);

	if (id > maxID) maxID = id;
	if (anim > maxID) maxID = anim;
}

static void init(jaw::properties* props) {
	input::clear();
	sprite::clear();
	anim::clear();

	animation = anim::create({
		.startFrame = 1,
		.endFrame = 3,
		.row = 2,
		.frameInterval = jaw::millis(200),
		.loop = true
	});

	jaw::argb* pixels;
	auto dim = asset::bmp("F:/C++/GameJam/assets.png", &pixels);
	if (pixels) {
		bitmap = draw::createBmp(dim);
		if (bitmap != jaw::INVALID_ID) {
			draw::writeBmp(bitmap, pixels, dim);
		}
	}

	input::bindLMBDown(createSprite);
	input::bindKeyDown(key::ESC, [](jaw::properties*) { engine::stop(); });
}

static void loop(jaw::properties* props) {
	snprintf(buf, sizeof(buf), "%u", maxID);

	draw::str(draw::strOptions{
		.rect = jaw::recti(jaw::vec2i(), props->winsize),
		.str = buf,
		.color = jaw::color::WHITE
	},
	1);
}

int main() {
	jaw::properties props;
	props.size = jaw::vec2i(300, 200);
	props.scale = 4;
	engine::start(&props, nullptr, init, loop);
}