#pragma once
#include "types.h"

namespace asset {
	size_t file(const char* filename, void** data);

	// Supports multiple file formats
	// PNG and BMP are recommended because they're lossless and support alpha
	jaw::vec2i bmp(const char* filename, jaw::argb** pixelData);
}