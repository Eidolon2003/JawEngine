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

	// Get the number of bytes remaining in the temporary allocator
	size_t tempallocBytesRemaining();

	// Attempt to map a circular buffer in virtual address space
	// buf[0] == buf[bytes] && buf[1] == buf[bytes+1] && etc.
	// The size of the buffer may be rounded up due to OS constraints
	void *mapCircularBuffer(size_t *bytes);

	// Unmap a circular buffer allocated with mapCircularBuffer
	// The bytes value must be the rounded value returned from mapCircularBuffer
	void unmapCircularBuffer(void *buffer, size_t bytes);

	// The engine will automatically call the callback after the given time has passed
	void setTimer(const jaw::properties *props, jaw::nanoseconds time, jaw::statefn callback);

	// Remove all active timers 
	void clearTimers();
}