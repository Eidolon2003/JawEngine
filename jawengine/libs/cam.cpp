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
#include "cam.h"
#include <cmath>	//floorf

namespace cam {
	static util::slotAllocator<cam::id, cam::Camera, cam::MAX_NUM> slots;

	void clear() {
		slots.clear();
	}

	cam::id create(const Camera &c) {
		return slots.create(&c);
	}

	void destroy(cam::id c) {
		slots.destroy(c);
	}

	Camera *idtoptr(cam::id c) {
		return slots.idtoptr(c);
	}

	// out = (in * mult) + offset
	jaw::vec2i forward(cam::id cid, jaw::vec2i v) {
		Camera *c = slots.idtoptr(cid);
		if (!c) return v; 
		
		auto f = jaw::vec2f(v) * c->multiplier;
		auto i = jaw::vec2i(f) + c->offset;
		return i;
	}
	jaw::recti forward(cam::id cid, jaw::recti r) {
		Camera *c = slots.idtoptr(cid);
		if (!c) return r;

		auto f = jaw::rectf(r) * c->multiplier;
		auto i = jaw::recti(f) + c->offset;
		return i;
	}

	// By algebra:
	// in = (out - offset) / mult
	jaw::vec2i reverse(cam::id cid, jaw::vec2i v) {
		Camera *c = slots.idtoptr(cid);
		if (!c) return v;

		auto f = jaw::vec2f(v - c->offset) / c->multiplier;
		f.x = floorf(f.x);
		f.y = floorf(f.y);
		auto i = jaw::vec2i(f);
		return i;
	}
	jaw::recti reverse(cam::id cid, jaw::recti r) {
		Camera *c = slots.idtoptr(cid);
		if (!c) return r;

		auto f = jaw::rectf(r - c->offset) / c->multiplier;
		f.tl.x = floorf(f.tl.x);
		f.tl.y = floorf(f.tl.y);
		f.br.x = floorf(f.br.x);
		f.br.y = floorf(f.br.y);
		auto i = jaw::recti(f);
		return i;
	}
}