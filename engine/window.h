#pragma once
#include "JawEngine.h"

#if defined WINDOWS

#include <Windows.h>

class Window {
public:
	Window(jaw::Application* pApp);
	~Window();

	jaw::GraphicsInterface* pGraphics;
	jaw::SoundInterface* pSound;
	jaw::InputInterface* pInput;
	jaw::Application* pApp;

private:
	WNDCLASSEX wc;
	HWND hWnd;

	static LRESULT __stdcall WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif