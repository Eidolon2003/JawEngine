#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"

#define SPEED 100

static jaw::sprid guy = jaw::INVALID_ID;

static jaw::bmpid bmpIdle = jaw::INVALID_ID;
static jaw::animdefid animIdle = jaw::INVALID_ID;

static jaw::bmpid bmpWalk = jaw::INVALID_ID;
static jaw::animdefid animWalk = jaw::INVALID_ID;

static void handleAnim(jaw::sprite* spr) {
	anim::destroy(spr->animState);

	if (spr->vel.x == 0) {
		spr->animState = anim::instanceOf(animIdle);
		spr->bmp = bmpIdle;
	}
	else {
		spr->animState = anim::instanceOf(animWalk);
		spr->bmp = bmpWalk;
	}
}

static void leftDown(jaw::properties* props) {
	jaw::sprite* spr = sprite::idtoptr(guy);
	if (!spr) return;

	spr->mirrorX = true;
	spr->vel.x -= SPEED;
	handleAnim(spr);
}

static void leftUp(jaw::properties* props) {
	jaw::sprite* spr = sprite::idtoptr(guy);
	if (!spr) return;

	spr->vel.x += SPEED;
	handleAnim(spr);
}

static void rightDown(jaw::properties* props) {
	jaw::sprite* spr = sprite::idtoptr(guy);
	if (!spr) return;

	spr->mirrorX = false;
	spr->vel.x += SPEED;
	handleAnim(spr);
}

static void rightUp(jaw::properties* props) {
	jaw::sprite* spr = sprite::idtoptr(guy);
	if (!spr) return;

	spr->vel.x -= SPEED;
	handleAnim(spr);
}

static void showBox(jaw::sprid id, jaw::properties* props) {
	jaw::sprite* spr = sprite::idtoptr(id);
	if (!spr) return;

	sprite::draw(id, props);
	draw::rect(
		draw::rectOptions{
			.rect = spr->rect(),
			.color = 0xFF555555
		},
		spr->z - 1
	);
}

static void init(jaw::properties* props) {
	jaw::argb* pixels;
	jaw::vec2i dim = asset::bmp("F:/assets/test-animation/idle-40x70x6.png", &pixels);
	if (pixels) {
		bmpIdle = draw::createBmp(dim);
		draw::writeBmp(bmpIdle, pixels, dim);
	}

	dim = asset::bmp("F:/assets/test-animation/walk-40x70x12.png", &pixels);
	if (pixels) {
		bmpWalk = draw::createBmp(dim);
		draw::writeBmp(bmpWalk, pixels, dim);
	}

	animIdle = anim::create({
		.startFrame = 0,
		.endFrame = 5,
		.frameInterval = jaw::millis(250),
		.loop = true
	});

	animWalk = anim::create({
		.startFrame = 0,
		.endFrame = 11,
		.frameInterval = jaw::millis(100),
		.loop = true
	});

	guy = sprite::create({
		.pos = (props->size / 2) - jaw::vec2i(20, 0),
		.z = 1,
		.mirrorX = false,
		.bmp = bmpIdle,
		.frameSize = { 40, 70 },
		.animState = anim::instanceOf(animIdle)
	});
	//sprite::customDraw(guy, showBox);

	input::bindKeyDown(key::A, leftDown);
	input::bindKeyUp(key::A, leftUp);
	input::bindKeyDown(key::D, rightDown);
	input::bindKeyUp(key::D, rightUp);
}

static void loop(jaw::properties* props) {}

int main() {
	jaw::properties props;
	props.title = "Animation Test";
	props.size = jaw::vec2i(400, 225);
	props.scale = 4;
	engine::start(&props, nullptr, init, loop);
}