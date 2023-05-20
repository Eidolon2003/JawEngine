#pragma once

#include "JawEngine.h"

#include <atomic>
#include <thread>

#if defined WINDOWS
#include <Windows.h>
#endif

namespace jaw {
	
	class Window {
	public:
		Window(AppInterface* pApp, EngineInterface* pEngine);
		~Window();

		GraphicsInterface* pGraphics;
		SoundInterface* pSound;
		InputInterface* pInput;
		EngineInterface* pEngine;
		AppInterface* pApp;

		std::atomic<bool> finished;

	private:
		void ThreadFunk();


#if defined WINDOWS
	public:
		WNDCLASSEX wc;
		HWND hWnd;

		static LRESULT __stdcall WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	};

};