#pragma once
#include "types.h"

namespace util {
	// returns an unaligned pointer to n bytes
	void *tempalloc(size_t n);

	// conveniently allocate num * sizeof(T) bytes aligned to alignof(T)
	// does not actually initialize the memory in any way
	template <typename T>
	T *tempalloc(size_t num) {
		constexpr auto align = alignof(T);
		void *p = tempalloc(num*sizeof(T) + align - 1);
		if (!p) return nullptr;
		uintptr_t aligned = (((uintptr_t)p + align - 1) / align) * align;
		return (T*)aligned;
	}

	size_t tempallocBytesRemaining();
}