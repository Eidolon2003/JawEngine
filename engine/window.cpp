#include "window.h"

jaw::Window::Window(jaw::AppInterface* pApp, const jaw::AppProperties& props, jaw::EngineInterface* pEngine) {
	finished.store(false);

	start = thisFrame = lastFrame = std::chrono::high_resolution_clock::now();

	this->pApp = pApp;
	this->pEngine = pEngine;

	framerate = props.framerate;
	sizeX = props.sizeX;
	sizeY = props.sizeY;
	repeat = props.enableKeyRepeat;

#if defined WINDOWS
	hWnd = NULL;
	wc = WNDCLASSEX();
#endif

	std::thread(&Window::ThreadFunk, this).detach();
}

jaw::Window::~Window() {
	delete pApp;
	delete pInput;
}

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


#if defined WINDOWS

void jaw::Window::ThreadFunk() {
	timeBeginPeriod(1);

	constexpr DWORD STYLE = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	RECT rect = { 0, 0, sizeX, sizeY };
	AdjustWindowRect(&rect, STYLE, false);

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
		STYLE,										//Window Style (not resizable)
		CW_USEDEFAULT, CW_USEDEFAULT,				//Location (Top left is 0,0)
		rect.right - rect.left,						//width of the window
		rect.bottom - rect.top,						//height of the window							
		NULL,										//Parent window
		NULL,										//Menu
		wc.hInstance,								//Instance Handle
		NULL										//Additional application data
	);

	//Set the window's data to a pointer to this object so it can be accessed in WinProc
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(hWnd, SW_SHOW);

	this->pSound = nullptr;
	this->pGraphics = new jaw::D2DGraphics(hWnd, sizeX, sizeY);
	this->pInput = new jaw::Input(repeat);

	pApp->pWindow = this;
	pApp->pEngine = pEngine;
	pApp->pGraphics = pGraphics;
	pApp->pSound = pSound;
	pApp->pInput = pInput;

	pApp->Init();

	bool running = true;
	while (running) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (FrameLimiter()) {
			pGraphics->BeginFrame();
			pApp->Loop();
			pGraphics->EndFrame();
			pInput->Reset();
		}
	}

	finished.store(true);
	timeEndPeriod(1);
}

LRESULT __stdcall jaw::Window::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	jaw::Window* _this = (jaw::Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(NULL);
		goto def;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN: {
		unsigned short x = lParam & 0xFFFF;
		unsigned short y = (lParam >> 16) & 0xFFFF;
		_this->pInput->mouse.x = *(short*)&x;
		_this->pInput->mouse.y = *(short*)&y;
		_this->pInput->mouse.flags = wParam;

		if (_this->pInput->clickDown)
			_this->pInput->clickDown(_this->pInput->mouse);

		goto def;
	}

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP: {
		unsigned short x = lParam & 0xFFFF;
		unsigned short y = (lParam >> 16) & 0xFFFF;
		_this->pInput->mouse.x = *(short*)&x;
		_this->pInput->mouse.y = *(short*)&y;
		_this->pInput->mouse.flags = wParam;

		if (_this->pInput->clickUp)
			_this->pInput->clickUp(_this->pInput->mouse);

		goto def;
	}

	case WM_MOUSEMOVE: {
		unsigned short x = lParam & 0xFFFF;
		unsigned short y = (lParam >> 16) & 0xFFFF;
		_this->pInput->mouse.x = *(short*)&x;
		_this->pInput->mouse.y = *(short*)&y;
		_this->pInput->mouse.flags = wParam;
		goto def;
	}

	case WM_MOUSEWHEEL: {
		unsigned short x = lParam & 0xFFFF;
		unsigned short y = (lParam >> 16) & 0xFFFF;
		_this->pInput->mouse.x = *(short*)&x;
		_this->pInput->mouse.y = *(short*)&y;
		_this->pInput->mouse.flags = wParam;
		_this->pInput->mouse.wheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		goto def;
	}

	case WM_CHAR:
		_this->pInput->charInput.push_back(wParam);
		goto def;

	case WM_KEYDOWN:
		//Check for repeat
		if ((lParam & 0x40000000) && !_this->pInput->enableKeyRepeat)
			goto def;

		//set appropriate bit
		_this->pInput->keybits[wParam >> 4 & 0xF] |= 1 << (wParam & 0xF);

		//call keybind
		if (_this->pInput->downJumpTable[wParam & 0xFF])
			_this->pInput->downJumpTable[wParam & 0xFF]();

		goto def;

	case WM_KEYUP:
		//reset appropriate bit
		_this->pInput->keybits[wParam >> 4 & 0xF] &= ~(1 << (wParam & 0xF));

		//call keybind
		if (_this->pInput->upJumpTable[wParam & 0xFF])
			_this->pInput->upJumpTable[wParam & 0xFF]();

		goto def;

	def:
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

#endif