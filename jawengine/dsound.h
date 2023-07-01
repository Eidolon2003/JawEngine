#pragma once
#include "JawEngine.h"
#include <Windows.h>
#include <wincodec.h>
#include <dsound.h>
#include <map>
#include <fstream>

namespace jaw {

	class DirectSound : public SoundInterface {
	private:
		struct WaveHeader {
			char chunkId[4];
			unsigned long chunkSize;
			char format[4];
			char subChunkId[4];
			unsigned long subChunkSize;
			unsigned short audioFormat;
			unsigned short numChannels;
			unsigned long sampleRate;
			unsigned long bytesPerSecond;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			char dataChunkId[4];
			unsigned long dataSize;
		};

		IDirectSound* pDirectSound;
		IDirectSoundBuffer* pPrimaryBuffer;
		IDirectSoundBuffer* pTempBuffer;

		WAVEFORMATEX secondaryFormat;
		DSBUFFERDESC secondaryDesc;
		std::map<std::string, IDirectSoundBuffer*> secondaryBuffers;

	public:
		DirectSound(HWND);
		~DirectSound();

		bool Load(std::string) override;
		bool Play(std::string) override;
		bool Loop(std::string) override;
		bool Stop(std::string) override;
		void StopAll() override;
	};

};