#include "window.h"

jaw::Window::Window(jaw::AppInterface* pApp, const jaw::AppProperties& props, jaw::EngineInterface* pEngine) {
	finished.store(false);

	start = thisFrame = lastFrame = std::chrono::high_resolution_clock::now();

	framerate = props.framerate;

	this->pApp = pApp;
	this->pEngine = pEngine;
	this->pGraphics = nullptr;
	this->pSound = nullptr;
	this->pInput = nullptr;

	pApp->pWindow = this;
	pApp->pEngine = pEngine;
	pApp->pGraphics = pGraphics;
	pApp->pSound = pSound;
	pApp->pInput = pInput;

#if defined WINDOWS
	hWnd = NULL;
	wc = WNDCLASSEX();
#endif

	std::thread(&Window::ThreadFunk, this).detach();
}

jaw::Window::~Window() {
	delete pApp;
}

#if defined WINDOWS

void jaw::Window::ThreadFunk() {
	timeBeginPeriod(1);

	wc.cbSize = sizeof(wc);
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Test";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(
		0,											//Optional styling
		wc.lpszClassName,							//Window Class
		wc.lpszClassName,							//Window Text
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,	//Window Style (not resizable)
		CW_USEDEFAULT, CW_USEDEFAULT,				//Location (Top left is 0,0)
		640,										//width of the window
		480,										//height of the window							
		NULL,										//Parent window
		NULL,										//Menu
		wc.hInstance,								//Instance Handle
		NULL										//Additional application data
	);
	ShowWindow(hWnd, SW_SHOW);

	bool running = true;
	while (running) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (FrameLimiter()) {
			pApp->Loop();
		}
	}

	finished.store(true);
	timeEndPeriod(1);
}

LRESULT __stdcall jaw::Window::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(NULL);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

#endif

bool jaw::Window::isClosed() {
	return finished.load();
}

bool jaw::Window::FrameLimiter() {
	using namespace std::chrono;

	auto now = high_resolution_clock::now();
	duration<uint64_t, std::nano> frametime = now - thisFrame;
	duration<uint64_t, std::nano> target = duration<uint64_t, std::nano>((uint64_t)(1000000000 / framerate));

	if (frametime + milliseconds(1) < target)
		std::this_thread::sleep_until(thisFrame + target - milliseconds(1));

	if (frametime < target) return false;

	lastFrame = thisFrame;
	thisFrame = now;
	return true;
}

std::chrono::duration<double, std::milli> jaw::Window::getFrametime() {
	return thisFrame - lastFrame;
}

std::chrono::duration <uint64_t, std::milli> jaw::Window::getLifetime() {
	using namespace std::chrono;
	return duration_cast<duration<uint64_t, std::milli>>(thisFrame - start);
}