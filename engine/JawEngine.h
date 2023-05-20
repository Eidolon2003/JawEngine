#pragma once

#if (defined (_WIN32) || defined (_WIN64))
#define WINDOWS
#elif (defined (LINUX) || defined (__linux__))
#define LINUX
#endif

#include <cstdint>

namespace jaw {

	struct EngineProperties {
		bool showCMD;
	};

	struct AppProperties {
		double framerate;
	};

	class GraphicsInterface {

	};

	class SoundInterface {

	};

	class InputInterface {

	};

	class AppInterface;
	class EngineInterface {
	public:
		virtual void OpenWindow(AppInterface*, const AppProperties&) = 0;
		virtual void CloseWindow(AppInterface*) = 0;
	};

	class AppInterface {
	public:
		GraphicsInterface* pGraphics = nullptr;
		SoundInterface* pSound = nullptr;
		InputInterface* pInput = nullptr;
		EngineInterface* pEngine = nullptr;

		virtual void Loop() = 0;
	};

	void StartEngine(AppInterface*, const AppProperties&, const EngineProperties&);

};