#include "win32_internal.h"

LRESULT __stdcall winproc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_CLOSE:
		PostQuitMessage(NULL);
		goto def;

	default:
	def:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

HWND win::init(jaw::properties *props) {
	timeBeginPeriod(1);

	HWND console = GetConsoleWindow();
	ShowWindow(console, props->showCMD ? SW_SHOW : SW_HIDE);

	DWORD style = 0;
	RECT rect {};
	struct { int x, y; } location = { 0,0 };
	switch (props->mode) {
	case jaw::properties::WINDOWED:
		style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		props->winsize = props->scaledSize();
		rect = { 0, 0, props->winsize.x, props->winsize.y };
		AdjustWindowRect(&rect, style, false);
		location = { CW_USEDEFAULT, CW_USEDEFAULT };
		break;

	case jaw::properties::WINDOWED_FULLSCREEN:
		style = WS_POPUP;
		props->winsize = jaw::vec2i(
			(int16_t)GetSystemMetrics(SM_CXSCREEN),
			(int16_t)GetSystemMetrics(SM_CYSCREEN)
		);

		if (props->size.x == 0)
			props->size.x = (int)(props->winsize.x / props->scale);
		if (props->size.y == 0)
			props->size.y = (int)(props->winsize.y / props->scale);

		rect = { 0, 0, props->winsize.x, props->winsize.y };
		location = { 0, 0 };
		break;
	}

	WNDCLASSEX wc {};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = winproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = props->title;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(
		0,											//Optional styling
		wc.lpszClassName,							//Window Class
		wc.lpszClassName,							//Window Text
		style,										//Window Style (not resizable)
		location.x, location.y,						//Location
		rect.right - rect.left,						//width of the window
		rect.bottom - rect.top,						//height of the window							
		NULL,										//Parent window
		NULL,										//Menu
		wc.hInstance,								//Instance Handle
		NULL										//Additional application data
	);
	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}

void win::deinit() {
	timeEndPeriod(1);
}