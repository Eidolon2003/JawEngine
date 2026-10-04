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

#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <mmsystem.h>	//timer
#include <cstdlib>
#include <cassert>
#include <list>
#include "../common/internal_utils.h"
#include "../../JawEngine.h"	// JAW_DBGPRINTF & utils.h

static LARGE_INTEGER countsPerSecond;
static TIMECAPS timerInfo;

struct timer {
	jaw::nanoseconds endTime;
	jaw::statefn callback;
};
static_assert(std::is_trivial_v<timer>);
static util::slotAllocator<uint32_t, timer, util::MAX_NUM_TIMERS> timers;

#ifndef NDEBUG
static size_t maxBytes = 0;
#endif

bool util::init(jaw::properties *props) {
	static auto _frameAllocator = util::arenaAllocator(props->frameAllocatorBytes);
	frameAllocator = &_frameAllocator;

	timers.clear();
	timeGetDevCaps(&timerInfo, sizeof(timerInfo));
	timeBeginPeriod(timerInfo.wPeriodMin);
	(void)QueryPerformanceFrequency(&countsPerSecond);
	return true;
}

void util::deinit() {
#ifndef NDEBUG
	JAW_DBGPRINT("frameAllocator used a maximum of " << maxBytes << " bytes");
#endif
	timers.clear();
	timeEndPeriod(timerInfo.wPeriodMin);
}

void util::beginFrame() {
#ifndef NDEBUG
	if (frameAllocator->bytesUsed() > maxBytes) maxBytes = frameAllocator->bytesUsed();
#endif
	frameAllocator->clear();
}

void *util::mapCircularBuffer(size_t *bytes) {
	// Round bytes up to allocation granularity
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	*bytes = (*bytes + (size_t)sysinfo.dwAllocationGranularity - 1) & ~((size_t)sysinfo.dwAllocationGranularity - 1);

	LPVOID view1, view2;
	HANDLE fileMapping;
	ULARGE_INTEGER size;
	size.QuadPart = *bytes;

	fileMapping = CreateFileMappingA(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		size.HighPart,
		size.LowPart,
		NULL
	);
	if (fileMapping == NULL) return nullptr;

	int attempts = 0;
	for (;;) {
		view1 = MapViewOfFile(
			fileMapping,
			FILE_MAP_ALL_ACCESS,
			0, 0,
			*bytes
		);
		if (!view1) goto fail;

		view2 = MapViewOfFileEx(
			fileMapping,
			FILE_MAP_ALL_ACCESS,
			0, 0,
			*bytes,
			(LPBYTE)view1 + *bytes
		);
		if (view2) break;

		UnmapViewOfFile(view1);
		if (++attempts > 16) goto fail;
	}

	CloseHandle(fileMapping);
	return view1;

fail:
	CloseHandle(fileMapping);
	return nullptr;
}

void util::unmapCircularBuffer(void *buffer, size_t bytes) {
	UnmapViewOfFile(buffer);
	UnmapViewOfFile((LPBYTE)buffer + bytes);
}

bool util::setTimer(const jaw::properties *props, jaw::nanoseconds time, jaw::statefn callback) {
	timer t = { .endTime = props->uptime + time, .callback = callback };
	uint32_t id = timers.create(&t);
	return id != jaw::INVALID_ID;
}

void util::clearTimers() {
	timers.clear();
}

void util::updateTimers(jaw::properties *props) {
	for (uint32_t slot = 0; slot < timers.nextSlot; slot++) {
		if (timers.isOpen[slot]) continue;
		uint32_t id = slot | timers.gens[slot];
		timer *t = timers.items + slot;
		if (props->uptime >= t->endTime) {
			t->callback(props);
			timers.destroy(id);
		}
	}
}

jaw::nanoseconds util::getTimePoint() {
	LARGE_INTEGER timePoint;
	auto _ = QueryPerformanceCounter(&timePoint);
	return timePoint.QuadPart * (1'000'000'000ULL / countsPerSecond.QuadPart);
}

jaw::nanoseconds util::accurateSleep(jaw::nanoseconds time, jaw::nanoseconds startPoint) {
	int msTimerAccuracy = timerInfo.wPeriodMin;
	int msSleepTime = (int)(((time / 1'000'000LL) / msTimerAccuracy) - 1) * msTimerAccuracy;
	if (msSleepTime > 0) Sleep((DWORD)msSleepTime);
	// time remaining to wait is less than 2x the timer accuracy
	// tried going for 1x timer accuracy, but it made frame pacing less consistent
	assert((time - (getTimePoint() - startPoint)) < (timerInfo.wPeriodMin * 2'000'000));
	jaw::nanoseconds retTime;
	while ((retTime = getTimePoint()) - startPoint < time);
	return retTime;
}

// arenaAllocator constructor implementation
util::arenaAllocator::arenaAllocator(size_t commitSize) {
	base = head = end = nullptr;

	void *alloc = VirtualAlloc(NULL, commitSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (alloc) {
		base = head = alloc;
		end = (char *)base + commitSize;
	}
}