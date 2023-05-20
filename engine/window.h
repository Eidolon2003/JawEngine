#pragma once

#include "JawEngine.h"

#include <atomic>
#include <thread>
#include <chrono>
#include <ratio>

#if defined WINDOWS
#include <Windows.h>
#endif

namespace jaw {
	
	class Window {
	public:
		Window(AppInterface* pApp, const AppProperties&, EngineInterface* pEngine);
		~Window();

		GraphicsInterface* pGraphics;
		SoundInterface* pSound;
		InputInterface* pInput;
		EngineInterface* pEngine;
		AppInterface* pApp;

		std::atomic<bool> finished;

	private:
		void ThreadFunk();
		bool FrameLimiter();

		double framerate;
		std::chrono::high_resolution_clock::time_point start, lastFrame;


#if defined WINDOWS
	public:
		HWND getHWND() { return hWnd; }

	private:
		WNDCLASSEX wc;
		HWND hWnd;

		static LRESULT __stdcall WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	};

};