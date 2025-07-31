#include <windows.h>
#include <memoryapi.h>

#include <cassert>
#include <string>
#include <unordered_map>

#include "asset.h"

struct FileInfo {
	size_t size;
	jaw::vec2i dim;	// For image files specifically, won't have a value for others
	void* rawData;
	void* processedData;
};

static std::unordered_map<std::string, FileInfo> fileCache;

size_t asset::file(const char* filename, void** data) {
	if (fileCache.contains(filename)) {
		auto& info = fileCache[filename];
		*data = info.rawData;
		return info.size;
	}

	HANDLE file = CreateFileA(
		filename,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (file == INVALID_HANDLE_VALUE) {
		*data = nullptr;
		return 0;
	}

	ULARGE_INTEGER size = {};
	size.LowPart = GetFileSize(file, &size.HighPart);
	assert(size.LowPart != INVALID_FILE_SIZE || size.HighPart != 0);

	HANDLE mapping = CreateFileMappingA(
		file,
		NULL,
		PAGE_READWRITE,
		size.HighPart,
		size.LowPart,
		NULL
	);
	assert(mapping != NULL);

	*data = MapViewOfFile(
		mapping,
		FILE_MAP_ALL_ACCESS,
		0, 0,
		(SIZE_T)(size.QuadPart)
	);
	if (*data == NULL) {
		CloseHandle(mapping);
		CloseHandle(file);
		return 0;
	}

	CloseHandle(file);
	CloseHandle(mapping);

	fileCache[filename] = { .size = (size_t)(size.QuadPart), .rawData = *data };

	return (size_t)(size.QuadPart);
}

jaw::vec2i asset::png(const char* filename, jaw::argb** outputData) {
	if (fileCache.contains(filename)) {
		auto& info = fileCache[filename];
		*outputData = (jaw::argb*)info.processedData;
		return info.dim;
	}

	void* rawData;
	size_t size = asset::file(filename, &rawData);
	if (rawData == nullptr) {
		*outputData = nullptr;
		return {};
	}

	// Allocating too much space for pixels now, read the PNG to actually determine this
	jaw::argb* pixels = (jaw::argb*)VirtualAlloc(
		NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);
	if (pixels == NULL) {
		*outputData = nullptr;
		return {};
	}

	// Process the PNG here

	auto& info = fileCache[filename];
	info.processedData = pixels;
	info.dim = {};	// Figure out the dimensions of the image from reading the PNG
	*outputData = pixels;
	return info.dim;	
}