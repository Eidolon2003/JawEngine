#include "internal_utils.h"
#include "../utils.h"
#include <cstdlib>
#include <cassert>

static char *arena;
static char *head;
static char *end;

#ifndef NDEBUG
#include <iostream>
static size_t maxBytes = 0;
#endif

bool util::init(jaw::properties *props) {
	arena = (char*)malloc(props->tempallocBytes);
	if (!arena) return false;
	head = arena;
	end = arena + props->tempallocBytes;
	return true;
}

void util::deinit() {
	free(arena);
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