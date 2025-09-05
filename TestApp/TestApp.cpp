#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"
#include <cstdio>

static jaw::animdefid animation;
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

void rectDraw(jaw::sprid spr, jaw::properties* props) {
	auto ptr = sprite::idtoptr(spr);
	if (!ptr) return;

	jaw::argb color;
	switch (anim::getFrame(ptr->animState)) {
	case 0:
		color = jaw::color::RED;
		break;
	case 1:
		color = jaw::color::GREEN;
		break;
	case 2:
		color = jaw::color::BLUE;
		break;
	default:
		assert(false);
	}

	draw::rect(draw::rectOptions{
		.rect = jaw::recti(ptr->pos, ptr->pos + 8),
		.color = color
	}, 0);
}

void createSprite(jaw::properties* props) {
	auto anim = anim::instanceOf(animation);
	auto id = sprite::create(jaw::sprite{
		.pos = props->mouse.pos.tofloat(),
		.vel = jaw::vec2f(0, 0),
		.animState = anim
	});

	sprite::customUpdate(id, timedDestroy);
	sprite::customDraw(id, rectDraw);

	if (id > maxID) maxID = id;
	if (anim > maxID) maxID = anim;
}

static void init(jaw::properties* props) {
	input::clear();
	sprite::clear();
	anim::clear();

	animation = anim::create({
		.startFrame = 0,
		.endFrame = 2,
		.frameInterval = jaw::millis(200),
		.loop = true
	});

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

	if (props->mouse.flags.lmb) createSprite(props);
}

int main() {
	jaw::properties props;
	props.size = jaw::vec2i(300, 200);
	props.scale = 4;
	engine::start(&props, nullptr, init, loop);
}