/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#include "../../headers/callback.h"
#include "internal_callback.h"
#include "../../headers/utils.h"	// slotAllocator

static util::slotAllocator<jaw::callbackid, jaw::callback, callback::MAX_NUM_CALLBACKS> callbacks;

void callback::init() {
	callbacks.clear();
}

jaw::callbackid callback::create(const jaw::callback &cb) {
	return callbacks.create(&cb);
}

void callback::destroy(jaw::callbackid id) {
	callbacks.destroy(id);
}

jaw::callback *callback::idtoptr(jaw::callbackid id) {
	return callbacks.idtoptr(id);
}

void callback::clear() {
	callbacks.clear();
}

// Call each callback each loop
void callback::loop(jaw::properties *p) {
	jaw::nanoseconds thisTime = util::getTimePoint();
	uint64_t thisFrame = p->framecount;

	for (jaw::callbackid i = 0; i < callbacks.nextSlot; i++) {
		if (callbacks.isOpen[i]) continue;

		jaw::callbackid id = i | callbacks.gens[i];
		jaw::callback *cb = callbacks.items + i;

		if (cb->frameInterval > 0) {
			if (thisFrame >= cb->frameInterval + cb->_prevFrame) {
				cb->_prevFrame = thisFrame;
				if (cb->callback) cb->callback(id, p);
			}
		}
		else if (cb->timeInterval > 0) {
			if (thisTime >= cb->timeInterval + cb->_prevTime) {
				cb->_prevTime = thisTime;
				if (cb->callback) cb->callback(id, p);
			}
		}
		else if (cb->callback) cb->callback(id, p);
	}
}