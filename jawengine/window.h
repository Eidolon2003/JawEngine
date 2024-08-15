#pragma once

#include "JawEngine.h"
#include "input.h"
#include "graphics.h"

#include <atomic>
#include <thread>

#if defined WINDOWS
#include <Windows.h>
#include "d2d.h"
#include "dsound.h"
#endif

namespace jaw {

	class Window : public WindowInterface {
	public:
		Window(AppInterface* app, const AppProperties&, EngineInterface* engine);
		~Window();

		bool isClosed();

		std::chrono::duration<double, std::milli> getFrametime() const override;
		std::chrono::duration<uint64_t, std::milli> getLifetime() const override;
		const AppProperties& getProperties() const override { return properties; }

	private:
		void ThreadFunk();
		bool FrameLimiter();
		void HandleSprites();

		AppProperties properties;
		jaw::Point winSize;
		std::chrono::high_resolution_clock::time_point start, thisFrame, lastFrame;

		SoundInterface* sound = nullptr;
		InternalGraphicsInterface* graphics = nullptr;
		Input* input = nullptr;
		EngineInterface* engine = nullptr;
		AppInterface* app = nullptr;

		std::atomic<bool> finished;


#if defined WINDOWS
	public:
		HWND getHWND() { return hWnd; }

	private:
		WNDCLASSEX wc = WNDCLASSEX();
		HWND hWnd = NULL;

		static LRESULT __stdcall WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void HandleMouse(LPARAM lParam, WPARAM wParam);
#endif

	};

};