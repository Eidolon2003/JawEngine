#include "window.h"

#if defined WINDOWS

LRESULT __stdcall Window::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(NULL);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

Window::Window(jaw::Application* pApp) {
	this->pApp = pApp;
	this->pGraphics = nullptr;
	this->pSound = nullptr;
	this->pInput = nullptr;

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
}

Window::~Window() {
	delete pApp;
}

#endif