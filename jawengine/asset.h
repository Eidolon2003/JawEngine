#pragma once
#include "types.h"

namespace asset {
	size_t file(const char* filename, void** data);
	jaw::vec2i png(const char* filename, jaw::argb** pixelData);
}