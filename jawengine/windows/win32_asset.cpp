#include <windows.h>
#include <memoryapi.h>
#include <wincodec.h>

#include <cassert>
#include <string>
#include <unordered_map>

#include "../asset.h"
#include "../common/internal_asset.h"

struct FileInfo {
	size_t size = 0;
	jaw::vec2i dim;	// For image files specifically, won't have a value for others
	void* rawData = nullptr;
	void* processedData = nullptr;
};

static std::unordered_map<std::string, FileInfo> fileCache;

static IWICImagingFactory* iwic;

void asset::init() {
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&iwic)
	);
	assert(SUCCEEDED(hr));
}

void asset::deinit() {
	iwic->Release();

	for (auto& [key, val] : fileCache) {
		auto x = UnmapViewOfFile(val.rawData);
		assert(x);

		if (val.processedData) {
			VirtualFree(val.processedData, 0, MEM_RELEASE);
		}
	}
}

size_t asset::file(const char* filename, void** data) {
	if (fileCache.contains(filename)) {
		auto& info = fileCache[filename];
		*data = info.rawData;
		return info.size;
	}

	HANDLE file = CreateFileA(
		filename,
		GENERIC_READ,
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
		PAGE_READONLY,
		size.HighPart,
		size.LowPart,
		NULL
	);
	assert(mapping != NULL);

	*data = MapViewOfFile(
		mapping,
		FILE_MAP_READ,
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

	fileCache[filename] = { 
		(size_t)(size.QuadPart),
		{0,0},
		*data,
		nullptr
	};

	return (size_t)(size.QuadPart);
}

jaw::vec2i asset::bmp(const char* filename, jaw::argb** outputData) {
	if (fileCache.contains(filename)) {
		auto& info = fileCache[filename];
		*outputData = (jaw::argb*)info.processedData;
		return info.dim;
	}

	void* rawData;
	size_t size = asset::file(filename, &rawData);
	if (rawData == nullptr) {
		// Couldn't open the file
		*outputData = nullptr;
		return {};
	}

	IWICStream* stream;
	HRESULT hr = iwic->CreateStream(&stream);
	assert(SUCCEEDED(hr));
	
	hr = stream->InitializeFromMemory((WICInProcPointer)rawData, (DWORD)size);
	assert(SUCCEEDED(hr));

	IWICBitmapDecoder* decoder;
	hr = iwic->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
	assert(SUCCEEDED(hr));

	IWICBitmapFrameDecode* frame;
	hr = decoder->GetFrame(0, &frame);
	assert(SUCCEEDED(hr));

	IWICFormatConverter* formatConverter;
	hr = iwic->CreateFormatConverter(&formatConverter);
	assert(SUCCEEDED(hr));

	hr = formatConverter->Initialize(
		frame,
		GUID_WICPixelFormat32bppBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	);
	assert(SUCCEEDED(hr));

	UINT x, y;
	formatConverter->GetSize(&x, &y);
	jaw::vec2i dim(x, y);

	jaw::argb* pixels = (jaw::argb*)VirtualAlloc(
		NULL, (x * y * sizeof(jaw::argb)), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);
	if (pixels == NULL) {
		*outputData = nullptr;
		return {};
	}

	hr = formatConverter->CopyPixels(
		nullptr,
		x * sizeof(jaw::argb),
		x * y * sizeof(jaw::argb),
		(BYTE*)pixels
	);
	assert(SUCCEEDED(hr));

	stream->Release();
	decoder->Release();
	frame->Release();
	formatConverter->Release();

	auto& info = fileCache[filename];
	info.processedData = pixels;
	info.dim = dim;
	*outputData = pixels;
	return info.dim;
}