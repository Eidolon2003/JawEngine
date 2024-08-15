#include "window.h"

jaw::Window::Window(jaw::AppInterface* app, const jaw::AppProperties& props, jaw::EngineInterface* engine) {
	finished.store(false);

	start = thisFrame = lastFrame = std::chrono::high_resolution_clock::now();

	this->app = app;
	this->engine = engine;

	properties = props;
	properties.layerCount = std::max(properties.layerCount, properties.backgroundCount);

	const float MAXFPS = 1000;
	if (properties.framerate < 0 || properties.framerate > MAXFPS)
		properties.framerate = MAXFPS;

	std::thread(&Window::ThreadFunk, this).detach();
}

jaw::Window::~Window() {
	delete input;
	delete graphics;
	delete sound;
	delete app;

	sprites.clear();
}

bool jaw::Window::isClosed() {
	return finished.load();
}

bool jaw::Window::FrameLimiter() {
	using namespace std::chrono;

	auto now = high_resolution_clock::now();
	duration<uint64_t, std::nano> frametime = now - thisFrame;

	//Uncapped
	if (properties.framerate == 0) {
		lastFrame = thisFrame;
		thisFrame = now;
		return true;
	}

	duration<uint64_t, std::nano> target = duration<uint64_t, std::nano>((uint64_t)(1000000000 / properties.framerate));

	//Check if the frametime is unusually high
	//Probably caused by the user moving the window
	//In this case, skip the next frame
	//This is to stop sprites from jumping too far in a single frame
	if (frametime > target * 10) {
		lastFrame = thisFrame;
		thisFrame = now;
		return false;
	}

	if (frametime + milliseconds(1) < target)
		std::this_thread::sleep_until(thisFrame + target - milliseconds(1));

	if (frametime < target) return false;

	lastFrame = thisFrame;
	thisFrame = now;
	return true;
}

std::chrono::duration<double, std::milli> jaw::Window::getFrametime() const {
	return thisFrame - lastFrame;
}

std::chrono::duration <uint64_t, std::milli> jaw::Window::getLifetime() const {
	using namespace std::chrono;
	return duration_cast<duration<uint64_t, std::milli>>(thisFrame - start);
}

#if defined WINDOWS

void jaw::Window::ThreadFunk() {
	timeBeginPeriod(1);

	DWORD style = 0;
	RECT rect = RECT();
	struct { int x, y; } location = { 0,0 };
	switch (properties.mode) {
	case AppProperties::WINDOWED:
		style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		winSize = jaw::Point(
			ScaleUp(properties.size.x, properties.scale),
			ScaleUp(properties.size.y, properties.scale)
		);
		rect = {
			0,
			0,
			winSize.x,
			winSize.y
		};
		AdjustWindowRect(&rect, style, false);
		location = { CW_USEDEFAULT, CW_USEDEFAULT };
		break;

	case AppProperties::WINDOWED_FULLSCREEN:
		style = WS_POPUP;
		winSize = jaw::Point(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

		if (properties.size.x == 0)
			properties.size.x = ScaleDown(winSize.x, properties.scale);
		if (properties.size.y == 0)
			properties.size.y = ScaleDown(winSize.y, properties.scale);

		rect = {
			0,
			0,
			winSize.x,
			winSize.y
		};
		location = { 0, 0 };
		break;
	}

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
	wc.lpszClassName = properties.title;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(
		0,											//Optional styling
		wc.lpszClassName,							//Window Class
		wc.lpszClassName,							//Window Text
		style,										//Window Style (not resizable)
		location.x, location.y,						//Location (Top left is 0,0)
		rect.right - rect.left,						//width of the window
		rect.bottom - rect.top,						//height of the window							
		NULL,										//Parent window
		NULL,										//Menu
		wc.hInstance,								//Instance Handle
		NULL										//Additional application data
	);
	ShowWindow(hWnd, SW_SHOW);

	//Set the window's data to a pointer to this object so it can be accessed in WinProc
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

	this->sound = new jaw::DirectSound(hWnd);
	this->graphics = new jaw::D2DGraphics(hWnd, properties, winSize, engine->getLocale());
	this->input = new jaw::Input(properties.enableKeyRepeat);

	app->window = this;
	app->engine = engine;
	app->graphics = graphics;
	app->sound = sound;
	app->input = input;

	app->Init();

	bool running = true;
	while (running) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (FrameLimiter()) {
			graphics->BeginFrame();
			app->Loop();
			HandleSprites();
			graphics->EndFrame();
			input->Reset();
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
	case WM_XBUTTONDOWN:
		_this->HandleMouse(lParam, wParam);
		if (_this->input->clickDown)
			_this->input->clickDown(_this->input->mouse);

		goto def;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		_this->HandleMouse(lParam, wParam);
		if (_this->input->clickUp)
			_this->input->clickUp(_this->input->mouse);

		goto def;

	case WM_MOUSEMOVE: 
	case WM_MOUSEWHEEL:
		_this->HandleMouse(lParam, wParam);
		goto def;

	case WM_CHAR:
		_this->input->charInput.push_back((wchar_t)wParam);
		goto def;

	case WM_KEYDOWN:
		//Check for repeat
		if ((lParam & 0x40000000) && !_this->input->enableKeyRepeat)
			goto def;

		//set appropriate bit
		_this->input->keybits[wParam >> 4 & 0xF] |= 1 << (wParam & 0xF);

		//call keybind
		if (_this->input->downJumpTable[wParam & 0xFF])
			_this->input->downJumpTable[wParam & 0xFF]();

		goto def;

	case WM_KEYUP:
		//reset appropriate bit
		_this->input->keybits[wParam >> 4 & 0xF] &= ~(1 << (wParam & 0xF));

		//call keybind
		if (_this->input->upJumpTable[wParam & 0xFF])
			_this->input->upJumpTable[wParam & 0xFF]();

		goto def;

	def:
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void jaw::Window::HandleMouse(LPARAM lParam, WPARAM wParam) {
	short x = lParam & 0xFFFF;
	short y = (lParam >> 16) & 0xFFFF;
	input->mouse.flags = wParam & 0xFF;
	input->mouse.wheel += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

	static const jaw::Point realSize(
		ScaleUp(properties.size.x, properties.scale),
		ScaleUp(properties.size.y, properties.scale)
	);

	static const jaw::Point offset(
		(winSize.x - realSize.x) / 2,
		(winSize.y - realSize.y) / 2
	);

	input->mouse.pos.x = ScaleDown(
		std::min(realSize.x, (int16_t)std::max(0, x - offset.x)),
		properties.scale
	);

	input->mouse.pos.y = ScaleDown(
		std::min(realSize.y, (int16_t)std::max(0, y - offset.y)),
		properties.scale
	);
}

#endif


/*
	SPRITES
*/

void jaw::Window::HandleSprites() {
	auto itr = sprites.begin();
	while (itr != sprites.end()) {
		auto spr = itr;
		itr++;
		if ((*spr)->Update(app)) {
			sprites.erase(spr);
		}
		else {
			(*spr)->Draw(app);
		}
	}
}