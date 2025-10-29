#pragma once
#include "types.h"

namespace asset {
	// This function is cached internally
	// Points to a read-only memory mapped file, do not write!
	const void *file(const char *filename, size_t *size);

	// This function is cached internally
	// Supports multiple file formats
	// PNG and BMP are recommended because they're lossless and support alpha
	jaw::argb *bmp(const char *filename, jaw::vec2i *dim);

	// This function is cached internally
	// Accepts 44.1kHz, 16 bit, 2 channel wav files
	int16_t *wav(const char *filename, size_t *numSamples);
}