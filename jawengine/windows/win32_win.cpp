#define _CRT_SECURE_NO_WARNINGS
#include "win32_internal_win.h"
#include "../common/internal_input.h"
#include <cmath>	//floorf, min
#include <cassert>
#include <windowsx.h>	//GET_X_LPARAM, GET_Y_LPARAM
#include <vector>

static void handleMouse(WPARAM wparam, LPARAM lparam, jaw::properties* props) {
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
	} [[fallthrough]];

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

	input::updateMouse(&mouse, props);
}

static LRESULT __stdcall winproc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	auto props = (jaw::properties*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

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
		handleMouse(wparam, lparam, props);
		return 0;

	case WM_CHAR: {
		wchar_t wc = (wchar_t)wparam;
		char c[5];
		assert(MB_CUR_MAX <= sizeof(c));
		auto len = wctomb(c, wc);
		assert(len == 1);
		if (len == 1) input::updateChar(c[0]);
	}	return 0;

	case WM_KEYDOWN: {
		if (lparam & (1LL << 30)) return 0;	//Ignore windows' key repeat
		uint8_t vkc = wparam & 0xFF;
		input::updateKey(vkc, true, props);
	}	return 0;

	case WM_KEYUP: {
		uint8_t vkc = wparam & 0xFF;
		input::updateKey(vkc, false, props);
	}	return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		// The only way I'm getting WM_PAINT that I've found is dragging the window off the
		// edge of the screen and then bringing it back on
		//assert(0);
		ValidateRect(hwnd, NULL);
		return 0;
					
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

static WNDCLASSEX wc;
static char className[256] = "JAWENGINE_WINDOW_CLASS_";

struct monitor {
	HMONITOR handle;
	MONITORINFOEX info;
};

static BOOL CALLBACK MonitorEnumProc(HMONITOR hmon, HDC hdc, LPRECT lprc, LPARAM data) {
	auto mons = (std::vector<monitor>*)(data);
	MONITORINFOEX info {};
	info.cbSize = sizeof(info);
	if (GetMonitorInfo(hmon, &info)) {
		mons->emplace_back(hmon, info);
	}
	return TRUE;
}

HWND win::init(jaw::properties *props) {
	// Show/Hide the console
	HWND console = GetConsoleWindow();
	ShowWindow(console, props->showCMD ? SW_SHOW : SW_HIDE);

	// Enumerate monitors and select the correct one
	std::vector<monitor> mons;
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&mons);
	assert(mons.size() > 0);

	props->monitorIndex = std::min(props->monitorIndex, (int)(mons.size() - 1));
	if (props->monitorIndex < 0) {
		// Find the primary monitor
		for (size_t i = 0; i < mons.size(); i++) {
			auto flags = mons[i].info.dwFlags;
			if (flags & MONITORINFOF_PRIMARY) {
				props->monitorIndex = (int)i;
				break;
			}
		}
	}
	assert(props->monitorIndex >= 0 && props->monitorIndex < mons.size());

	RECT monitorRect = mons[props->monitorIndex].info.rcMonitor;
	jaw::vec2i monitorTopLeft((int16_t)monitorRect.left, (int16_t)monitorRect.top);
	jaw::vec2i monitorBottomRight((int16_t)monitorRect.right, (int16_t)monitorRect.bottom);
	jaw::vec2i monitorDim = monitorBottomRight - monitorTopLeft;

	DWORD style = 0;
	RECT rect {};
	jaw::vec2i loc = monitorTopLeft;
	switch (props->mode) {
	case jaw::properties::WINDOWED: {
		style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		props->winsize = props->scaledSize();
		rect = { 0, 0, props->winsize.x, props->winsize.y };
		AdjustWindowRect(&rect, style, false);
		loc = loc + ((monitorDim - props->winsize) * 0.5f);	// Center the window
	}	break;

	case jaw::properties::FULLSCREEN_CENTERED: {
		style = WS_POPUP;
		props->winsize = monitorDim;

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
	}	break;

	case jaw::properties::FULLSCREEN_CENTERED_INTEGER: {
		style = WS_POPUP;
		props->winsize = monitorDim;

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
	}	break;

	case jaw::properties::FULLSCREEN_STRETCHED: {
		style = WS_POPUP;
		props->winsize = monitorDim;

		if (props->size.x <= 0)
			props->size.x = props->winsize.x;
		if (props->size.y <= 0)
			props->size.y = props->winsize.y;

		props->scale = 1.f;

		rect = { 0, 0, props->winsize.x, props->winsize.y };
	}	break;
	}

	strncat(className, props->title, 256 - strlen(props->title));

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
		loc.x, loc.y,								//Location
		rect.right - rect.left,						//width of the window
		rect.bottom - rect.top,						//height of the window							
		NULL,										//Parent window
		NULL,										//Menu
		wc.hInstance,								//Instance Handle
		NULL										//Additional application data
	);
	ShowWindow(hwnd, SW_SHOW);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)props);

	return hwnd;
}

void win::deinit() {
	UnregisterClassA(wc.lpszClassName, wc.hInstance);
}
