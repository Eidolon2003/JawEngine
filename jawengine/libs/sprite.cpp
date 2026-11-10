/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "sprite.h"

namespace sprite {
	static util::slotAllocator<sprite::id, sprite::Sprite, sprite::MAX_NUM> slots;
	static jaw::callbackid spriteCallback = jaw::INVALID_ID;

	static void updateAndDrawAllSprites(jaw::callbackid cbkid, jaw::properties *props) {
		for (sprite::id i = 0; i < slots.nextSlot; i++) {
			if (slots.isOpen[i]) continue;
			sprite::Sprite *spr = slots.items + i;
			spr->updateAndDraw(spr, props);
		}
	}

	sprite::id create(const sprite::Sprite &s) {
		sprite::Sprite spr = s;
		spr._lastAnimationAge = spr.age = 0;
		spr._animationCounter = 0;
		if (spr.updateAndDraw == nullptr) spr.updateAndDraw = sprite::updateAndDraw;
		if (spr.scale <= 0) spr.scale = 1;

		if (spriteCallback == jaw::INVALID_ID) {
			spriteCallback = callback::create({
				.callback = updateAndDrawAllSprites
			});
		}

		sprite::id sprid = slots.create(&spr);
		return sprid;
	}

	void destroy(sprite::id sprid) {
		slots.destroy(sprid);
	}

	sprite::Sprite *idtoptr(sprite::id sprid) {
		return slots.idtoptr(sprid);
	}

	void clear() {
		slots.clear();
		callback::destroy(spriteCallback);
		spriteCallback = jaw::INVALID_ID;
	}

	void updateAndDraw(sprite::Sprite *spr, jaw::properties *props) {
		if (spr == nullptr) return;

		// update age and position
		spr->age += props->totalFrametime;
		spr->pos = spr->pos + (spr->vel * jaw::to_seconds(props->totalFrametime));

		// update animation at the correct interval
		if (spr->animationInterval > 0 &&
			spr->age >= spr->_lastAnimationAge + spr->animationInterval)
		{
			spr->_lastAnimationAge = spr->age;
			spr->_animationCounter++;
			if (spr->_animationCounter >= (spr->fullSize.x / spr->frameSize.x))
				spr->_animationCounter = 0;
		}

		if (spr->bmp == jaw::INVALID_ID) {
			draw::enqueue<draw::rect>({
				.rect = spr->rect(),
				.color = jaw::color::MAGENTA
			}, spr->z);
		}
		else {
			auto srcPoint = jaw::vec2i(
				spr->frameSize.x * spr->_animationCounter,
				spr->frameSize.y * spr->animationRow
			);
			auto srcRect = jaw::recti(
				srcPoint,
				srcPoint + spr->frameSize
			);
			draw::enqueue<draw::bmp>({
				.bmp = spr->bmp,
				.src = srcRect,
				.dest = spr->rect()
			}, spr->z);
		}
 	}
}