#include "dsound.h"

jaw::DirectSound::DirectSound(HWND hwnd) {
	secondaryBuffers = {};

	pDirectSound = nullptr;
	auto dummy = DirectSoundCreate(NULL, &pDirectSound, NULL);
	pDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

	pPrimaryBuffer = nullptr;
	DSBUFFERDESC primaryDesc;
	primaryDesc.dwSize = sizeof(DSBUFFERDESC);
	primaryDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	primaryDesc.dwBufferBytes = 0;
	primaryDesc.dwReserved = 0;
	primaryDesc.lpwfxFormat = NULL;
	primaryDesc.guid3DAlgorithm = GUID_NULL;
	pDirectSound->CreateSoundBuffer(&primaryDesc, &pPrimaryBuffer, NULL);

	WAVEFORMATEX primaryFormat;
	primaryFormat.wFormatTag = WAVE_FORMAT_PCM;
	primaryFormat.nSamplesPerSec = 44100;
	primaryFormat.wBitsPerSample = 16;
	primaryFormat.nChannels = 2;
	primaryFormat.nBlockAlign = (primaryFormat.wBitsPerSample / 8) * primaryFormat.nChannels;
	primaryFormat.nAvgBytesPerSec = primaryFormat.nSamplesPerSec * primaryFormat.nBlockAlign;
	primaryFormat.cbSize = 0;
	pPrimaryBuffer->SetFormat(&primaryFormat);

	secondaryFormat.wFormatTag = WAVE_FORMAT_PCM;
	secondaryFormat.nSamplesPerSec = 44100;
	secondaryFormat.wBitsPerSample = 16;
	secondaryFormat.nChannels = 2;
	secondaryFormat.nBlockAlign = (secondaryFormat.wBitsPerSample / 8) * secondaryFormat.nChannels;
	secondaryFormat.nAvgBytesPerSec = secondaryFormat.nSamplesPerSec * secondaryFormat.nBlockAlign;
	secondaryFormat.cbSize = 0;

	secondaryDesc.dwSize = sizeof(DSBUFFERDESC);
	secondaryDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	secondaryDesc.dwBufferBytes = NULL;
	secondaryDesc.dwReserved = 0;
	secondaryDesc.lpwfxFormat = &secondaryFormat;
	secondaryDesc.guid3DAlgorithm = GUID_NULL;
}

jaw::DirectSound::~DirectSound() {
	for (auto [s, x] : secondaryBuffers)
		x->Release();
	pPrimaryBuffer->Release();
	pDirectSound->Release();
}

bool jaw::DirectSound::Load(std::string filename) {
	std::fstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open()) return false;

	WaveHeader header;
	file.seekg(0);
	file.read((char*)(&header), sizeof(WaveHeader));

	if (strncmp(header.format, "WAVE", 4) != 0) return false;
	if (strncmp(header.chunkId, "RIFF", 4) != 0) return false;
	if (strncmp(header.subChunkId, "fmt", 3) != 0) return false;
	if (header.audioFormat != WAVE_FORMAT_PCM) return false;
	if (header.numChannels != 2) return false;
	if (header.sampleRate != 44100) return false;
	if (header.bitsPerSample != 16) return false;
	if (strncmp(header.dataChunkId, "data", 4) != 0) return false;

	secondaryDesc.dwBufferBytes = header.dataSize;
	file.seekg(sizeof(WaveHeader));
	char* data = new char[header.dataSize];
	file.read(data, header.dataSize);
	file.close();

	secondaryBuffers[filename] = nullptr;
	pTempBuffer = nullptr;
	pDirectSound->CreateSoundBuffer(&secondaryDesc, &pTempBuffer, NULL);
	pTempBuffer->QueryInterface(IID_IDirectSoundBuffer, (LPVOID*)&secondaryBuffers[filename]);
	pTempBuffer->Release();
	pTempBuffer = nullptr;

	char* pBuffer = nullptr;
	unsigned bufferSize;
	secondaryBuffers[filename]->Lock(0, header.dataSize, (LPVOID*)&pBuffer, (DWORD*)&bufferSize, NULL, 0, 0);
	memcpy(pBuffer, data, header.dataSize);
	secondaryBuffers[filename]->Unlock((LPVOID)pBuffer, bufferSize, NULL, 0);

	delete[] data;
	data = nullptr;
	return true;
}

bool jaw::DirectSound::Play(std::string filename) {
	if (!secondaryBuffers[filename] && !Load(filename))
		return false;

	secondaryBuffers[filename]->SetCurrentPosition(0);
	secondaryBuffers[filename]->SetVolume(-1000);
	secondaryBuffers[filename]->Play(0, 0, 0);
	return true;
}

bool jaw::DirectSound::Loop(std::string filename) {
	if (!secondaryBuffers[filename] && !Load(filename))
		return false;

	secondaryBuffers[filename]->SetCurrentPosition(0);
	secondaryBuffers[filename]->SetVolume(-1000);
	secondaryBuffers[filename]->Play(0, 0, DSBPLAY_LOOPING);
	return true;
}

bool jaw::DirectSound::Stop(std::string filename) {
	if (!secondaryBuffers[filename]) return false;
	secondaryBuffers[filename]->Stop();
	return true;
}

void jaw::DirectSound::StopAll() {
	for (auto [s, x] : secondaryBuffers)
		x->Stop();
}