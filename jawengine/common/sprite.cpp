#include "../sprite.h"
#include "internal_sprite.h"
#include "../draw.h"
#include <cstring>	//memcpy

struct animState {
	jaw::animdefid animation = jaw::INVALID_ID;
	unsigned frame = 0;
	jaw::nanoseconds animTimer = 0;
	bool finished = false;
};

/*
	ANIMATION
*/

static jaw::animation animDefs[anim::MAX_NUM_ANIM];
static size_t numAnimDef;

static animState animStates[anim::MAX_NUM_ANIM];
static size_t numAnimState;

void anim::clear() {
	numAnimDef = numAnimState = 0;
}

jaw::animdefid anim::create(const jaw::animation& ad) {
	if (numAnimDef == anim::MAX_NUM_ANIM) return jaw::INVALID_ID;
	memcpy(animDefs + numAnimDef, &ad, sizeof(jaw::animation));
	return (jaw::animdefid)numAnimDef++;
}

jaw::animation* anim::idtoptr(jaw::animdefid id) {
	if (id >= numAnimDef) return nullptr;
	return animDefs + id;
}

jaw::animstateid anim::instanceOf(jaw::animdefid id) {
	if (numAnimState == anim::MAX_NUM_ANIM) return jaw::INVALID_ID;
	animStates[id] = { .animation = id };
	return (jaw::animstateid)numAnimState++;
}

bool anim::finished(jaw::animstateid id) {
	if (id >= numAnimState) return false;
	return animStates[id].finished;
}


/*
	SPRITES
*/

static jaw::sprite sprites[sprite::MAX_NUM_SPR];
static size_t numSpr;

static jaw::sprfn updates[sprite::MAX_NUM_SPR];
static jaw::sprfn draws[sprite::MAX_NUM_SPR];

void sprite::clear() {
	numSpr = 0;
}

jaw::sprid sprite::create(const jaw::sprite& spr) {
	if (numSpr == sprite::MAX_NUM_SPR) return jaw::INVALID_ID;
	memcpy(sprites + numSpr, &spr, sizeof(jaw::sprite));
	return (jaw::sprid)numSpr++;
}

jaw::sprite* sprite::idtoptr(jaw::sprid id) {
	if (id >= numSpr) return nullptr;
	return sprites + id;
}

void sprite::customUpdate(jaw::sprid id, jaw::sprfn fn) {
	if (id >= numSpr) return;
	updates[id] = fn;
}

void sprite::customDraw(jaw::sprid id, jaw::sprfn fn) {
	if (id >= numSpr) return;
	draws[id] = fn;
}

void sprite::update(jaw::sprite* spr, jaw::properties* props) {
	spr->age += props->totalFrametime;
	spr->pos = spr->pos + (spr->vel * jaw::to_seconds(props->totalFrametime));
}

void sprite::draw(jaw::sprite* spr, jaw::properties* props) {
	if (spr->bmp == jaw::INVALID_ID) return;

	if (spr->animState == jaw::INVALID_ID) {
		draw::bmp(
			draw::bmpOptions{
				.bmp = spr->bmp,
				.src = jaw::recti(jaw::vec2i(), spr->frameSize),
				.dest = spr->rect()
			},
			spr->z
		);
		return;
	}

	const auto& state = animStates[spr->animState];
	const auto& def = animDefs[state.animation];
	auto tl = jaw::vec2i(
		spr->frameSize.x * state.frame,
		spr->frameSize.y * def.row
	);
	auto src = jaw::recti(tl, tl + spr->frameSize);

	draw::bmp(
		draw::bmpOptions{
			.bmp = spr->bmp,
			.src = src,
			.dest = spr->rect()
		},
		spr->z
	);
}

void sprite::tick(jaw::properties* props) {
	// Update animations
	for (size_t i = 0; i < numAnimState; i++) {
		auto& state = animStates[i];
		const auto& def = animDefs[state.animation];

		state.animTimer += props->totalFrametime;
		if (state.animTimer >= def.frameInterval) {
			state.animTimer = 0;
			if (++state.frame > def.endFrame) {
				if (def.loop) {
					state.frame = def.startFrame;
				}
				else {
					state.frame = def.endFrame;
					state.finished = true;
				}
				
			}
		}
	}

	// Update and draw sprites
	for (size_t i = 0; i < numSpr; i++) {
		if (updates[i])
			updates[i](sprites + i, props);
		else
			update(sprites + i, props);

		if (draws[i])
			draws[i](sprites + i, props);
		else
			draw(sprites + i, props);
	}
}