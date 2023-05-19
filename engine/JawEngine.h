#pragma once

#if (defined (_WIN32) || defined (_WIN64))
#define WINDOWS
#elif (defined (LINUX) || defined (__linux__))
#define LINUX
#endif

#include <thread>
#include <unordered_map>

namespace jaw {

	class GraphicsInterface {

	};

	class SoundInterface {

	};

	class InputInterface {

	};

	class Application {
	public:
		GraphicsInterface* pGraphics;
		SoundInterface* pSound;
		InputInterface* pInput;

		virtual void Loop() = 0;
	};

	class Engine {
	public:
		Engine();
		void OpenWindow(Application*);
		void CloseWindow(Application*);

	private:
		std::unordered_map<Application*, std::thread*> pWindowThreads;
	};

};