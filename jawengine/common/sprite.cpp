#include "../sprite.h"
#include "internal_sprite.h"
#include "../draw.h"
#include <cassert>
#include <cstring>	//memcpy

struct animState {
	jaw::animdefid animation = jaw::INVALID_ID;
	uint32_t frame = 0;
	jaw::nanoseconds animTimer = 0;
	bool finished = false;
};

/*
	ANIMATION
*/

static jaw::animation animDefs[anim::MAX_NUM_ANIM];
static size_t numAnimDef;

static animState animStates[anim::MAX_NUM_ANIM];
static jaw::animstateid openStates[anim::MAX_NUM_ANIM];
static bool isStateOpen[anim::MAX_NUM_ANIM];
static size_t nextStateID;
static size_t numStatesOpen;

void anim::clear() {
	numAnimDef = 0;
	nextStateID = 0;
	numStatesOpen = 0;
	memset(isStateOpen, 0, sizeof(isStateOpen));
}

jaw::animdefid anim::create(const jaw::animation &ad) {
	if (numAnimDef == anim::MAX_NUM_ANIM) return jaw::INVALID_ID;
	memcpy(animDefs + numAnimDef, &ad, sizeof(jaw::animation));
	return (jaw::animdefid)numAnimDef++;
}

void anim::destroy(jaw::animstateid stateID) {
	if (stateID >= nextStateID || isStateOpen[stateID]) return;
	openStates[numStatesOpen++] = stateID;
	isStateOpen[stateID] = true;
}

jaw::animation *anim::idtoptr(jaw::animdefid defID) {
	if (defID >= numAnimDef) return nullptr;
	return animDefs + defID;
}

jaw::animstateid anim::instanceOf(jaw::animdefid defID) {
	if (nextStateID == anim::MAX_NUM_ANIM && numStatesOpen == 0) return jaw::INVALID_ID;

	jaw::animstateid newStateID;
	if (numStatesOpen > 0) {
		newStateID = openStates[--numStatesOpen];
		isStateOpen[newStateID] = false;
	}
	else {
		newStateID = (jaw::animstateid)nextStateID++;
	}

	animStates[newStateID] = { .animation = defID, .frame = animDefs[defID].startFrame };
	return newStateID;
}

bool anim::finished(jaw::animstateid stateID) {
	if (stateID >= nextStateID || isStateOpen[stateID]) return false;
	return animStates[stateID].finished;
}

uint32_t anim::getFrame(jaw::animstateid stateID) {
	if (stateID >= nextStateID || isStateOpen[stateID]) return UINT32_MAX;
	return animStates[stateID].frame;
}


/*
	SPRITES
*/

static jaw::sprite sprites[sprite::MAX_NUM_SPR];
static jaw::sprid openSprites[sprite::MAX_NUM_SPR];
static bool isSpriteOpen[sprite::MAX_NUM_SPR];
static size_t nextSpriteID;
static size_t numSpritesOpen;

static jaw::sprfn updates[sprite::MAX_NUM_SPR];
static jaw::sprfn draws[sprite::MAX_NUM_SPR];

void sprite::clear() {
	nextSpriteID = 0;
	numSpritesOpen = 0;
	memset(isSpriteOpen, 0, sizeof(isSpriteOpen));
	memset(updates, 0, sizeof(updates));
	memset(draws, 0, sizeof(draws));
}

jaw::sprid sprite::create(const jaw::sprite &spr) {
	if (nextSpriteID == sprite::MAX_NUM_SPR && numSpritesOpen == 0) return jaw::INVALID_ID;

	jaw::sprid id;
	if (numSpritesOpen > 0) {
		id = openSprites[--numSpritesOpen];
		isSpriteOpen[id] = false;
	}
	else {
		id = (jaw::sprid)nextSpriteID++;
	}

	memcpy(sprites + id, &spr, sizeof(jaw::sprite));
	return id;
}

void sprite::destroy(jaw::sprid id) {
	if (id >= nextSpriteID || isSpriteOpen[id]) return;
	openSprites[numSpritesOpen++] = id;
	updates[id] = nullptr;
	draws[id] = nullptr;
	isSpriteOpen[id] = true;
}

jaw::sprite *sprite::idtoptr(jaw::sprid id) {
	if (id >= nextSpriteID || isSpriteOpen[id]) return nullptr;
	return sprites + id;
}

void sprite::customUpdate(jaw::sprid id, jaw::sprfn fn) {
	if (id >= nextSpriteID || isSpriteOpen[id]) return;
	updates[id] = fn;
}

void sprite::customDraw(jaw::sprid id, jaw::sprfn fn) {
	if (id >= nextSpriteID || isSpriteOpen[id]) return;
	draws[id] = fn;
}

void sprite::update(jaw::sprid id, jaw::properties *props) {
	auto spr = idtoptr(id);
	assert(spr);
	spr->age += props->totalFrametime;
	spr->pos = spr->pos + (spr->vel * jaw::to_seconds(props->totalFrametime));
}

void sprite::draw(jaw::sprid id, jaw::properties *props) {
	auto spr = idtoptr(id);
	if (!spr || spr->bmp == jaw::INVALID_ID) return;

	if (spr->animState == jaw::INVALID_ID || isStateOpen[spr->animState]) {
		draw::bmp(
			draw::bmpOptions{
				.bmp = spr->bmp,
				.src = jaw::recti(jaw::vec2i(), spr->frameSize),
				.dest = spr->rect(),
				.mirrorX = spr->mirrorX,
				.mirrorY = spr->mirrorY
			},
			spr->z
		);
		return;
	}

	const auto &state = animStates[spr->animState];
	assert(state.animation < numAnimDef);
	const auto &def = animDefs[state.animation];
	auto tl = jaw::vec2i(
		spr->frameSize.x * state.frame,
		spr->frameSize.y * def.row
	);
	auto src = jaw::recti(tl, tl + spr->frameSize);

	draw::bmp(
		draw::bmpOptions{
			.bmp = spr->bmp,
			.src = src,
			.dest = spr->rect(),
			.mirrorX = spr->mirrorX,
			.mirrorY = spr->mirrorY
		},
		spr->z
	);
}

void sprite::tick(jaw::properties *props) {
	// Update animations
	for (size_t i = 0; i < nextStateID; i++) {
		if (isStateOpen[i]) continue;

		auto &state = animStates[i];
		assert(state.animation < numAnimDef);
		const auto &def = animDefs[state.animation];

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
	for (jaw::sprid i = 0; i < nextSpriteID; i++) {
		if (isSpriteOpen[i]) continue;

		if (updates[i])
			updates[i](i, props);
		else
			update(i, props);

		if (draws[i])
			draws[i](i, props);
		else
			draw(i, props);
	}
}