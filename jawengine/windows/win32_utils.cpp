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
#include "../utils.h"

static char *arena;
static char *head;
static char *end;

static LARGE_INTEGER countsPerSecond;
static TIMECAPS timerInfo;

struct timer {
	jaw::nanoseconds endTime;
	jaw::statefn callback;
};
static_assert(std::is_trivial_v<timer>);
static std::list<timer> timerList;


#ifndef NDEBUG
#include <iostream>
static size_t maxBytes = 0;
#endif

bool util::init(jaw::properties *props) {
	arena = (char*)VirtualAlloc(NULL, props->tempallocBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!arena) return false;
	head = arena;
	end = arena + props->tempallocBytes;
	timerList.clear();
	timeGetDevCaps(&timerInfo, sizeof(timerInfo));
	timeBeginPeriod(timerInfo.wPeriodMin);
	auto b = QueryPerformanceFrequency(&countsPerSecond);
	return true;
}

void util::deinit() {
	VirtualFree(arena, 0, MEM_RELEASE);
	arena = head = end = nullptr;
#ifndef NDEBUG
	std::cout << "Debug: tempalloc used a maximum of " << maxBytes << " bytes.\n";
#endif
	timerList.clear();
	timeEndPeriod(timerInfo.wPeriodMin);
}

void util::beginFrame() {
#ifndef NDEBUG
	if ((size_t)(head - arena) > maxBytes) maxBytes = (head - arena);
#endif
	head = arena;
}

void *util::tempalloc(size_t bytes) {
	if (head + bytes > end) {
		assert(false);
		return nullptr;
	}
	char *old = head;
	head += bytes;
	return old;
}

size_t util::tempallocBytesRemaining() {
	return (size_t)(end - head);
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

void util::setTimer(const jaw::properties *props, jaw::nanoseconds time, jaw::statefn callback) {
	timerList.emplace_back(props->uptime + time, callback);
}

void util::clearTimers() {
	timerList.clear();
}

void util::updateTimers(jaw::properties *props) {
	auto it = timerList.begin();
	while (it != timerList.end()) {
		if (props->uptime >= it->endTime) {
			// Time is up
			it->callback(props);
			timerList.erase(it++);
		}
		else {
			it++;
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