#pragma once

#include "JawEngine.h"
#include "input.h"
#include "graphics.h"

#include <atomic>
#include <thread>

#if defined WINDOWS
#include <Windows.h>
#include "d2d.h"
#endif

namespace jaw {
	
	class Window : public WindowInterface {
	public:
		Window(AppInterface* pApp, const AppProperties&, EngineInterface* pEngine);
		~Window();

		bool isClosed();

		std::chrono::duration<double, std::milli> getFrametime() override;
		std::chrono::duration<uint64_t, std::milli> getLifetime() override;
		void RegisterSprite(std::shared_ptr<Sprite>) override;

	private:
		void ThreadFunk();
		bool FrameLimiter();
		void HandleSprites();

		uint16_t sizeX;
		uint16_t sizeY;
		double framerate;
		bool repeat;
		std::chrono::high_resolution_clock::time_point start, thisFrame, lastFrame;

		SoundInterface* pSound;
		InternalGraphicsInterface* pGraphics;
		Input* pInput;
		EngineInterface* pEngine;
		AppInterface* pApp;

		std::vector<std::shared_ptr<Sprite>> sprites;

		std::atomic<bool> finished;


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