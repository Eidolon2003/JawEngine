#include "JawEngine.h"

static bool running = true;

void engine::stop() {
	running = false;
}

void limiter(jaw::properties* props) {
	using namespace std::chrono;

	//framerate of 0 means unlimited
	if (props->framerate == 0) {
		auto now = high_resolution_clock::now();
		props->frametime = now - props->lastframe;
		props->uptime += props->frametime;
		props->lastframe = now;
		return;
	}

	auto now = high_resolution_clock::now();
	auto frametime = now - props->lastframe;
	auto target_frametime = duration<uint64_t, std::nano>((uint64_t)(1'000'000'000 / props->framerate));

	//ignore abnormally large frametimes, caused by things like moving the window
	if (frametime > target_frametime * 10) {
		frametime = seconds(0);
	}
	else if (frametime > target_frametime) {
		//We're slower than target
		props->frametime = frametime;
		props->uptime += frametime;
		props->lastframe = now;
		return;
	}

	//Coarse sleep followed by short spin-loop
	std::this_thread::sleep_until(props->lastframe + target_frametime - milliseconds(1));
	do {
		now = high_resolution_clock::now();
		frametime = now - props->lastframe;
	} while (frametime < target_frametime);
	props->frametime = frametime;
	props->uptime += frametime;
	props->lastframe = now;
}

#ifdef JAW_WINDOWS
#include "windraw_internal.h"
#include "win32_internal.h"

void engine::start(jaw::properties* props) {
	HWND hwnd = win::init(props);
	draw::init(props, hwnd);
	game::init();

	do {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		game::loop();
		draw::prepareRender();
		draw::render();
		ValidateRect(hwnd, NULL);
		props->framecount++;
		limiter(props);
	} while (running);

	draw::deinit();
	win::deinit();
}
#endif