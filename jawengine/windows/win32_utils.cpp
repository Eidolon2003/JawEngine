#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <cstdlib>
#include <cassert>
#include "../common/internal_utils.h"
#include "../utils.h"

static char *arena;
static char *head;
static char *end;

#ifndef NDEBUG
#include <iostream>
static size_t maxBytes = 0;
#endif

bool util::init(jaw::properties *props) {
	arena = (char*)VirtualAlloc(NULL, props->tempallocBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!arena) return false;
	head = arena;
	end = arena + props->tempallocBytes;
	return true;
}

void util::deinit() {
	VirtualFree(arena, 0, MEM_RELEASE);
	arena = head = end = nullptr;
#ifndef NDEBUG
	std::cout << "Debug: tempalloc used a maximum of " << maxBytes << " bytes.\n";
#endif
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