#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <memoryapi.h>
#include <wincodec.h>

#include <cassert>
#include <string>
#include <unordered_map>

#include "../asset.h"
#include "../common/internal_asset.h"

// This is for compatbility with mingw on Linux
#ifdef __MINGW32__
typedef BYTE *WICInProcPointer;
#endif

static wchar_t wstrBuffer[1024];
static size_t towstrbuf(const char *str) {
	if (str == nullptr) return static_cast<size_t>(-1);
	return mbstowcs(wstrBuffer, str, 1024);
}

struct FileInfo {
	size_t size = 0;
	void *data = nullptr;
};
static std::unordered_map<std::string, FileInfo> fileCache;

struct BmpInfo {
	jaw::vec2i dim;
	jaw::argb *px = nullptr;
};
static std::unordered_map<std::string, BmpInfo> bmpCache;

struct WavInfo {
	int16_t *samples;
	size_t num;
};
static std::unordered_map<std::string, WavInfo> wavCache;

static IWICImagingFactory *iwic;

void asset::init() {
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&iwic)
	);
	assert(SUCCEEDED(hr));
}

void asset::deinit() {
	iwic->Release();
	iwic = nullptr;

	for (auto &[key, val] : fileCache) UnmapViewOfFile(val.data);
	fileCache.clear();

	for (auto &[key, val] : bmpCache) VirtualFree(val.px, 0, MEM_RELEASE);
	bmpCache.clear();

	for (auto &[key, val] : wavCache) VirtualFree(val.samples, 0, MEM_RELEASE);
	wavCache.clear();
}

static void *mapFile(const char *filename, size_t *fileSize) {
	HANDLE file{};
	ULARGE_INTEGER size{};
	HANDLE mapping{};
	void *data;

	file = CreateFileA(
		filename,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (file == INVALID_HANDLE_VALUE) goto fail0;

	size.LowPart = GetFileSize(file, &size.HighPart);
	if (size.LowPart == INVALID_FILE_SIZE && size.HighPart == 0) goto fail1;

	mapping = CreateFileMappingA(
		file,
		NULL,
		PAGE_READONLY,
		0, 0,
		NULL
	);
	if (mapping == NULL) goto fail1;

	data = MapViewOfFile(
		mapping,
		FILE_MAP_READ,
		0, 0,
		(SIZE_T)(size.QuadPart)
	);
	if (data == NULL) goto fail2;

	CloseHandle(mapping);
	CloseHandle(file);
	*fileSize = (size_t)size.QuadPart;
	return data;

fail2:
	CloseHandle(mapping);
fail1:
	CloseHandle(file);
fail0:
	*fileSize = 0;
	return nullptr;
}

const void *asset::file(const char *filename, size_t *size) {
	if (fileCache.contains(filename)) {
		auto &cache = fileCache[filename];
		*size = cache.size;
		return cache.data;
	}

	void *data = mapFile(filename, size);
	if (!data) return nullptr;

	fileCache[filename] = { 
		.size = *size,
		.data = data
	};

	return data;
}

jaw::argb *asset::bmp(const char *filename, jaw::vec2i *dim) {
	if (bmpCache.contains(filename)) {
		auto &cache = bmpCache[filename];
		*dim = cache.dim;
		return cache.px;
	}

	jaw::argb *px;

	IWICStream *stream;
	HRESULT hr = iwic->CreateStream(&stream);
	if (FAILED(hr)) goto fail0;

	towstrbuf(filename);
	hr = stream->InitializeFromFilename(wstrBuffer, GENERIC_READ);
	if (FAILED(hr)) goto fail1;

	IWICBitmapDecoder *decoder;
	hr = iwic->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
	if (FAILED(hr)) goto fail1;

	IWICBitmapFrameDecode *frame;
	hr = decoder->GetFrame(0, &frame);
	if (FAILED(hr)) goto fail2;

	IWICFormatConverter *formatConverter;
	hr = iwic->CreateFormatConverter(&formatConverter);
	if (FAILED(hr)) goto fail3;

	hr = formatConverter->Initialize(
		frame,
		GUID_WICPixelFormat32bppBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	);
	if (FAILED(hr)) goto fail4;

	UINT x, y;
	formatConverter->GetSize(&x, &y);
	*dim = jaw::vec2i(x, y);

	px = (jaw::argb*)VirtualAlloc(
		NULL, (sizeof(jaw::argb)*dim->product()), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);
	if (px == NULL) goto fail4;

	hr = formatConverter->CopyPixels(
		nullptr,
		x * sizeof(jaw::argb),
		x * y * sizeof(jaw::argb),
		(BYTE*)px
	);
	if (FAILED(hr)) goto fail5;

	formatConverter->Release();
	frame->Release();
	decoder->Release();
	stream->Release();

	bmpCache[filename] = {
		.dim = *dim,
		.px = px
	};
	return px;


fail5:
	VirtualFree(px, 0, MEM_RELEASE);
fail4:
	formatConverter->Release();
fail3:
	frame->Release();
fail2:
	decoder->Release();
fail1:
	stream->Release();
fail0:
	*dim = jaw::vec2i();
	return nullptr;
}

// This is a super bad wav parser
// It doesn't properly handle different RIFF chunks, instead assuming only fmt and data
// We also probably want to support other audio formats in the future
static int16_t *parseWav(const char *filename, size_t *numSamples) {
#pragma pack(push, 1)
	// https://en.wikipedia.org/wiki/WAV#WAV_file_header
	struct WaveHeader {
		char FileTypeBlocID[4];
		uint32_t FileSize;
		char FileFormatID[4];
		char FormatBlocID[4];
		uint32_t BlocSize;
		uint16_t AudioFormat;
		uint16_t NbrChannels;
		uint32_t Frequency;
		uint32_t BytePerSec;
		uint16_t BytePerBloc;
		uint16_t BitsPerSample;
		char DataBlocID[4];
		uint32_t DataSize;
	};
#pragma pack(pop)

	size_t fileBytes;
	void *fileData = mapFile(filename, &fileBytes);
	if (!fileData) goto fail0;

	WaveHeader *header; 
	header = (WaveHeader*)fileData;

	// Check file header "magic values"
	if (strncmp(header->FileTypeBlocID, "RIFF", 4) != 0) goto fail1;
	if (strncmp(header->FileFormatID, "WAVE", 4) != 0) goto fail1;
	if (strncmp(header->FormatBlocID, "fmt", 3) != 0) goto fail1;
	if (strncmp(header->DataBlocID, "data", 4) != 0) goto fail1;

	// PCM Integer format
	if (header->AudioFormat != 1) goto fail1;

	// 2 Channels
	if (header->NbrChannels != 2) goto fail1;

	// 44.1 kHz
	if (header->Frequency != 44100) goto fail1;

	// 16 bits per sample
	if (header->BitsPerSample != 16) goto fail1;

	// Make sure the size is consistent
	if (sizeof(WaveHeader) + header->DataSize > fileBytes) goto fail1;

	// If we got here then we know the file is in the proper format
	int16_t *samples;
	samples = (int16_t*)VirtualAlloc(
		NULL, header->DataSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
	);
	if (samples == NULL) goto fail1;

	memcpy(samples, (char*)fileData + sizeof(WaveHeader), header->DataSize);
	*numSamples = header->DataSize * 8/header->BitsPerSample;
	UnmapViewOfFile(fileData);
	return samples;

fail1:
	UnmapViewOfFile(fileData);
fail0:
	*numSamples = 0;
	return nullptr;
}

int16_t *asset::wav(const char *filename, size_t *numSamples) {
	if (wavCache.contains(filename)) {
		auto &cache = wavCache[filename];
		*numSamples = cache.num;
		return cache.samples;
	}

	int16_t *samples = parseWav(filename, numSamples);
	if (!samples) return nullptr;

	wavCache[filename] = {
		.samples = samples,
		.num = *numSamples
	};
	return samples;
}