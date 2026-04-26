/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025 Julian Williams
 *
 * JawEngine 0.2.0
 * https://github.com/Eidolon2003/JawEngine
 */

#include "../sprite.h"
#include "internal_sprite.h"
#include "../draw.h"
#include "../utils.h"
#include <cassert>

struct animState {
	jaw::animdefid animation;
	uint32_t frame;
	jaw::nanoseconds animTimer;
	bool finished;
};
static_assert(std::is_trivial_v<animState>);

/*
	ANIMATION
*/

static jaw::animation animDefs[anim::MAX_NUM_ANIM];
static size_t numAnimDef;

static util::slotAllocator<jaw::animstateid, animState, anim::MAX_NUM_ANIM> animStates;

void anim::clear() {
	numAnimDef = 0;
	animStates.clear();
}

jaw::animdefid anim::create(const jaw::animation &ad) {
	if (numAnimDef == anim::MAX_NUM_ANIM) return jaw::INVALID_ID;
	memcpy(animDefs + numAnimDef, &ad, sizeof(jaw::animation));
	return (jaw::animdefid)numAnimDef++;
}

void anim::destroy(jaw::animstateid stateID) {
	animStates.destroy(stateID);
}

jaw::animation *anim::idtoptr(jaw::animdefid defID) {
	if (defID >= numAnimDef) return nullptr;
	return animDefs + defID;
}

jaw::animstateid anim::instanceOf(jaw::animdefid defID) {
	if (defID >= numAnimDef) return jaw::INVALID_ID;

	animState data{
		.animation = defID,
		.frame = animDefs[defID].startFrame
	};

	return animStates.create(&data);
}

bool anim::finished(jaw::animstateid stateID) {
	animState *state = animStates.idtoptr(stateID);
	if (state == nullptr) return false;
	else return state->finished;
}

uint32_t anim::getFrame(jaw::animstateid stateID) {
	animState *state = animStates.idtoptr(stateID);
	if (state == nullptr) return UINT32_MAX;
	else return state->frame;
}


/*
	SPRITES
*/

static util::slotAllocator<jaw::sprid, jaw::sprite, sprite::MAX_NUM_SPR> sprites;
static jaw::sprfn updates[sprite::MAX_NUM_SPR];
static jaw::sprfn draws[sprite::MAX_NUM_SPR];

void sprite::clear() {
	sprites.clear();
	memset(updates, 0, sizeof(updates));
	memset(draws, 0, sizeof(draws));
}

jaw::sprid sprite::create(const jaw::sprite &spr) {
	return sprites.create(&spr);
}

void sprite::destroy(jaw::sprid id) {
	if (sprites.destroy(id)) {
		updates[id%sprite::MAX_NUM_SPR] = nullptr;
		draws[id%sprite::MAX_NUM_SPR] = nullptr;
	}
}

jaw::sprite *sprite::idtoptr(jaw::sprid id) {
	return sprites.idtoptr(id);
}

void sprite::customUpdate(jaw::sprid id, jaw::sprfn fn) {
	if (sprites.idtoptr(id) == nullptr) return;
	updates[id%sprite::MAX_NUM_SPR] = fn;
}

void sprite::customDraw(jaw::sprid id, jaw::sprfn fn) {
	if (sprites.idtoptr(id) == nullptr) return;
	draws[id%sprite::MAX_NUM_SPR] = fn;
}

void sprite::update(jaw::sprid id, jaw::properties *props) {
	auto spr = sprites.idtoptr(id);
	assert(spr);
	spr->age += props->totalFrametime;
	spr->pos = spr->pos + (spr->vel * jaw::to_seconds(props->totalFrametime));
}

void sprite::draw(jaw::sprid id, jaw::properties *props) {
	auto spr = sprites.idtoptr(id);
	if (!spr || spr->bmp == jaw::INVALID_ID) return;

	auto state = animStates.idtoptr(spr->animState);
	if (spr->animState == jaw::INVALID_ID || state == nullptr) {
		draw::enqueue(
			draw::bmp{
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

	assert(state->animation < numAnimDef);
	const auto &def = animDefs[state->animation];
	auto tl = jaw::vec2i(
		spr->frameSize.x * state->frame,
		spr->frameSize.y * def.row
	);
	auto src = jaw::recti(tl, tl + spr->frameSize);

	draw::enqueue(
		draw::bmp{
			.bmp = spr->bmp,
			.src = src,
			.dest = spr->rect(),
			.mirrorX = spr->mirrorX,
			.mirrorY = spr->mirrorY
		},
		spr->z
	);
}

void sprite::updateAll(jaw::properties *props) {
	// Update animations
	for (size_t i = 0; i < animStates.nextSlot; i++) {
		if (animStates.isOpen[i]) continue;

		auto state = animStates.items + i;
		assert(state->animation < numAnimDef);
		const auto &def = animDefs[state->animation];

		state->animTimer += props->totalFrametime;
		if (state->animTimer >= def.frameInterval) {
			state->animTimer = 0;
			if (++state->frame > def.endFrame) {
				if (def.loop) {
					state->frame = def.startFrame;
				}
				else {
					state->frame = def.endFrame;
					state->finished = true;
				}
				
			}
		}
	}

	// Update sprites
	for (jaw::sprid i = 0; i < sprites.nextSlot; i++) {
		if (sprites.isOpen[i]) continue;

		if (updates[i])
			updates[i](i + sprites.gens[i], props);
		else
			update(i + sprites.gens[i], props);
	}
}

void sprite::drawAll(jaw::properties *props) {
	// Draw sprites
	for (jaw::sprid i = 0; i < sprites.nextSlot; i++) {
		if (sprites.isOpen[i]) continue;

		if (draws[i])
			draws[i](i + sprites.gens[i], props);
		else
			draw(i + sprites.gens[i], props);
	}
}