#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../sound.h"
#include "../common/internal_sound.h"

#include <windows.h>
#include <xaudio2.h>

static IXAudio2* xaudio;
static IXAudio2MasteringVoice* master;

static IXAudio2SourceVoice* sounds[sound::MAX_NUM_SOUNDS];
static size_t nextID;

#define CHANNELS 2
#define SAMPLE_RATE 44100
#define SAMPLE_DEPTH 16
#define BLOCK_ALIGN (((CHANNELS) * (SAMPLE_DEPTH)) / 8)

static WAVEFORMATEX format{
	.wFormatTag = WAVE_FORMAT_PCM,
	.nChannels = CHANNELS,
	.nSamplesPerSec = SAMPLE_RATE,
	.nAvgBytesPerSec = SAMPLE_RATE * BLOCK_ALIGN,
	.nBlockAlign = BLOCK_ALIGN,
	.wBitsPerSample = SAMPLE_DEPTH,
	.cbSize = 0
};

typedef HRESULT (WINAPI *XAudio2Create_t)(IXAudio2** ppXAudio2, UINT Flags, XAUDIO2_PROCESSOR XAudio2Processor);

static HMODULE dll;
static XAudio2Create_t pXAudio2Create;

void sound::init() {
	dll = LoadLibraryA("xaudio2_8.dll");
	if (!dll) {
		return;
	}

	pXAudio2Create = (XAudio2Create_t)GetProcAddress(dll, "XAudio2Create");
	if (!pXAudio2Create) {
		FreeLibrary(dll);
		return;
	}

	auto hr = pXAudio2Create(&xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		FreeLibrary(dll);
		return;
	}

	hr = xaudio->CreateMasteringVoice(&master);
	if (FAILED(hr)) {
		xaudio->Release();
		FreeLibrary(dll);
		return;
	}
}

void sound::deinit() {
	for (size_t i = 0; i < nextID; i++) {
		sounds[i]->DestroyVoice();
	}
	master->DestroyVoice();
	xaudio->Release();
}

jaw::soundid sound::create() {
	if (nextID == sound::MAX_NUM_SOUNDS) return jaw::INVALID_ID;
	if (FAILED(xaudio->CreateSourceVoice(sounds + nextID, &format))) {
		return jaw::INVALID_ID;
	}
	return (jaw::soundid)nextID++;
}

bool sound::write(jaw::soundid id, int16_t* data, size_t size, bool loop) {
	if (id >= nextID || !data) return false;

	XAUDIO2_BUFFER buffer{
		.Flags = XAUDIO2_END_OF_STREAM,
		.AudioBytes = (UINT32)size,
		.pAudioData = (const BYTE*)data,
		.PlayBegin = 0,
		.PlayLength = 0,
		.LoopBegin = 0,
		.LoopLength = 0,
		.LoopCount = loop ? (UINT32)XAUDIO2_LOOP_INFINITE : 0,
		.pContext = nullptr
	};

	sounds[id]->Stop();
	sounds[id]->FlushSourceBuffers();
	sounds[id]->SubmitSourceBuffer(&buffer);
	return true;
}

void sound::start(jaw::soundid id) {
	if (id >= nextID) return;
	sounds[id]->Start(0);
}

void sound::stop(jaw::soundid id) {
	if (id >= nextID) return;
	sounds[id]->Stop();
}
