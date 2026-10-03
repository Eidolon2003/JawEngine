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

#pragma once
#include "types.h"

// Includes for allocators.h
#include <cstring>	// memcpy, memset
#include <cassert>
#include <type_traits>

namespace util {
	// util::slotAllocator, util::arenaAllocator
#include "allocators.h"

	// Frame-local arena allocator
	// Automatically cleared by the engine at the end of the frame
	inline arenaAllocator *frameAllocator;

	// The maximum number of timers that can exist at once
	constexpr size_t MAX_NUM_TIMERS = 32;

	// The engine will automatically call the callback after the given time has passed
	// Returns false if the maximum number of timers was exceeded
	bool setTimer(const jaw::properties *props, jaw::nanoseconds time, jaw::statefn callback);

	// Remove all active timers 
	void clearTimers();

	// Wrapper for QueryPerformanceCounter on Windows
	jaw::nanoseconds getTimePoint();

	// Sleep as close to the target as possible then spin for the remaining time
	jaw::nanoseconds accurateSleep(jaw::nanoseconds time, jaw::nanoseconds startPoint);

	// Attempt to map a circular buffer in virtual address space
	// buf[0] == buf[bytes] && buf[1] == buf[bytes+1] && etc.
	// The size of the buffer may be rounded up due to OS constraints
	void *mapCircularBuffer(size_t *bytes);

	// Unmap a circular buffer allocated with mapCircularBuffer
	// The bytes value must be the rounded value returned from mapCircularBuffer
	void unmapCircularBuffer(void *buffer, size_t bytes);
}