#define _CRT_SECURE_NO_WARNINGS
#include "win32_internal_win.h"
#include "internal_input.h"
#include <cmath>	//floorf, min
#include <cassert>
#include <windowsx.h>	//GET_X_LPARAM, GET_Y_LPARAM

static jaw::properties* props;

void handleMouse(WPARAM wparam, LPARAM lparam) {
	jaw::mouse mouse;
	mouse.pos.x = GET_X_LPARAM(lparam);
	mouse.pos.y = GET_Y_LPARAM(lparam);
	mouse.flags.all = wparam & 0xFF;
	mouse.wheelDelta = GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;

	switch (props->mode) {
	case jaw::properties::FULLSCREEN_CENTERED:
	case jaw::properties::FULLSCREEN_CENTERED_INTEGER: {
		const jaw::vec2i centerOffset = ((props->winsize - props->scaledSize()) / (int16_t)2);
		mouse.pos = mouse.pos - centerOffset;
	}	// This falls through intentionally!

	case jaw::properties::WINDOWED: {
		mouse.pos = mouse.pos / props->scale;

		mouse.pos.x = std::max(
			(int16_t)0,
			std::min(
				(int16_t)(props->size.x - 1),
				mouse.pos.x
			)
		);
		mouse.pos.y = std::max(
			(int16_t)0,
			std::min(
				(int16_t)(props->size.y - 1),
				mouse.pos.y
			)
		);

	}	break;

	case jaw::properties::FULLSCREEN_STRETCHED: {
		mouse.pos.x = (int16_t)((float)mouse.pos.x / ((float)props->winsize.x / (float)props->size.x));
		mouse.pos.y = (int16_t)((float)mouse.pos.y / ((float)props->winsize.y / (float)props->size.y));
	} break;
	}

	//The computed mouse coordinate must fall inside the window
	assert(mouse.pos >= 0 && mouse.pos < props->size);

	input::updateMouse(&mouse);
}

LRESULT __stdcall winproc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		handleMouse(wparam, lparam);
		return 0;

	case WM_CHAR: {
		wchar_t wc = (wchar_t)wparam;
		char c[4];
		assert(MB_CUR_MAX <= sizeof(c));
		auto len = wctomb(c, wc);
		assert(len == 1);
		if (len == 1) input::updateChar(c[0]);
	}	return 0;

	case WM_KEYDOWN: {
		if (lparam & (1 << 30)) return 0;	//Ignore windows' key repeat
		uint8_t vkc = wparam & 0xFF;
		input::updateKey(vkc, true);
	}	return 0;

	case WM_KEYUP: {
		uint8_t vkc = wparam & 0xFF;
		input::updateKey(vkc, false);
	}	return 0;

	case WM_CLOSE:
		PostQuitMessage(NULL);
		return 0;

	case WM_PAINT:
		//The engine is calling ValidateRect and redrawing the whole window every frame
		//Don't expect to receive any paint messages
		assert(0);
		ValidateRect(hwnd, NULL);
		return 0;
					
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

static WNDCLASSEX wc;
static char className[256] = "JAWENGINE_WINDOW_CLASS_";

HWND win::init(jaw::properties *p) {
	props = p;
	HWND console = GetConsoleWindow();
	ShowWindow(console, props->showCMD ? SW_SHOW : SW_HIDE);

	DWORD style = 0;
	RECT rect {};
	struct { int x, y; } location = { 0,0 };
	switch (props->mode) {
	case jaw::properties::WINDOWED: {
		style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		props->winsize = props->scaledSize();
		rect = { 0, 0, props->winsize.x, props->winsize.y };
		AdjustWindowRect(&rect, style, false);
		location = { CW_USEDEFAULT, CW_USEDEFAULT };
	}	break;

	case jaw::properties::FULLSCREEN_CENTERED: {
		style = WS_POPUP;
		props->winsize = jaw::vec2i(
			(int16_t)GetSystemMetrics(SM_CXSCREEN),
			(int16_t)GetSystemMetrics(SM_CYSCREEN)
		);

		if (props->scale > 0) {
			if (props->size.x <= 0)
				props->size.x = (int)(props->winsize.x / props->scale);
			if (props->size.y <= 0)
				props->size.y = (int)(props->winsize.y / props->scale);
		}
		else {
			if (props->size.x <= 0)
				props->size.x = props->winsize.x;
			if (props->size.y <= 0)
				props->size.y = props->winsize.y;
		}

		if (props->scale <= 0) {
			props->scale = std::min(
				(float)props->winsize.x / props->size.x,
				(float)props->winsize.y / props->size.y
			);
		}

		rect = { 0, 0, props->winsize.x, props->winsize.y };
		location = { 0, 0 };
	}	break;

	case jaw::properties::FULLSCREEN_CENTERED_INTEGER: {
		style = WS_POPUP;
		props->winsize = jaw::vec2i(
			(int16_t)GetSystemMetrics(SM_CXSCREEN),
			(int16_t)GetSystemMetrics(SM_CYSCREEN)
		);

		if (props->scale > 0) {
			if (props->size.x <= 0)
				props->size.x = (int)(props->winsize.x / props->scale);
			if (props->size.y <= 0)
				props->size.y = (int)(props->winsize.y / props->scale);
		}
		else {
			if (props->size.x <= 0)
				props->size.x = props->winsize.x;
			if (props->size.y <= 0)
				props->size.y = props->winsize.y;
		}

		if (props->scale <= 0) {
			props->scale = std::min(
				(float)props->winsize.x / props->size.x,
				(float)props->winsize.y / props->size.y
			);
			props->scale = floorf(props->scale);
		}

		rect = { 0, 0, props->winsize.x, props->winsize.y };
		location = { 0, 0 };
	}	break;

	case jaw::properties::FULLSCREEN_STRETCHED: {
		style = WS_POPUP;
		props->winsize = jaw::vec2i(
			(int16_t)GetSystemMetrics(SM_CXSCREEN),
			(int16_t)GetSystemMetrics(SM_CYSCREEN)
		);

		if (props->size.x <= 0)
			props->size.x = props->winsize.x;
		if (props->size.y <= 0)
			props->size.y = props->winsize.y;

		props->scale = 1.f;

		rect = { 0, 0, props->winsize.x, props->winsize.y };
		location = { 0, 0 };
	}	break;
	}

	strncat(className, props->title, 256);

	wc = {};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = winproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);	//TODO: Custom icon support
	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(
		0,											//Optional styling
		wc.lpszClassName,							//Window Class
		props->title,								//Window Text
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
	UnregisterClassA(wc.lpszClassName, wc.hInstance);
}