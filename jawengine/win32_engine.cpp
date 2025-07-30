#include "JawEngine.h"
#include "win32_internal_draw.h"
#include "win32_internal_win.h"
#include "internal_input.h"

static bool running = true;
static LARGE_INTEGER countsPerSecond;
static TIMECAPS timerInfo;
static jaw::nanoseconds startPoint, lastFrame, thisFrame;

jaw::nanoseconds getTimePoint() {
	LARGE_INTEGER timePoint;
	auto _ = QueryPerformanceCounter(&timePoint);
	return timePoint.QuadPart * (1'000'000'000ULL / countsPerSecond.QuadPart);
}

jaw::nanoseconds accurateSleep(jaw::nanoseconds time, jaw::nanoseconds startPoint) {
	int msTimerAccuracy = timerInfo.wPeriodMin;
	int msSleepTime = (int)(((time / 1'000'000LL) / msTimerAccuracy) - 1) * msTimerAccuracy;
	if (msSleepTime > 0) Sleep((DWORD)msSleepTime);
	// time remaining to wait is less then 2x the timer accuracy
	// tried going for 1x timer accuracy, but it made frame pacing less consistent
	assert((time - (getTimePoint() - startPoint)) < (timerInfo.wPeriodMin * 2'000'000));
	jaw::nanoseconds retTime;
	while ((retTime = getTimePoint()) - startPoint < time);
	return retTime;
}

void prelimit(jaw::properties* props) {
	// Record how long the frame took to process before any kind of limiting
	props->logicFrametime = getTimePoint() - lastFrame;
}

void limiter(jaw::properties* props) {
	props->framecount++;
	thisFrame = getTimePoint();
	assert(thisFrame > lastFrame);

	jaw::nanoseconds thisFrametime = thisFrame - lastFrame;
	jaw::nanoseconds prevFrametime = props->totalFrametime;
	jaw::nanoseconds targetFrametime = (jaw::nanoseconds)(1'000'000'000.0 / props->targetFramerate);

	if (thisFrametime >= prevFrametime * 5 && prevFrametime != 0) {
		// Detect abnormally large frametime spikes
		// Probably caused by something like the user moving the window
		// We don't want the game to include this time because it would cause a huge time jump
		// Fudge it by assuming this frame took the same as the previous (how to do better?)
		startPoint += thisFrametime - prevFrametime;
		//Don't need this assignment because it's already true
		//props->totalFrametime = prevFrametime;
		lastFrame = thisFrame;
		return;
	}

	if (props->targetFramerate <= 0) {
		// VSync is enabled, and we already waited for it
		props->totalFrametime = thisFrame - lastFrame;
		lastFrame = thisFrame;
		return;
	}

	if (thisFrametime >= targetFrametime) {
		// We've already taken too long, so no waiting required
		// This is bad!
		props->totalFrametime = thisFrametime;
		lastFrame = thisFrame;
		return;
	}

	// Here we know we need to sleep for some time to hit the target framerate
	thisFrame = accurateSleep(targetFrametime - thisFrametime, thisFrame);
	props->totalFrametime = thisFrame - lastFrame;
	assert(props->totalFrametime >= targetFrametime);
	lastFrame = thisFrame;
	return;
}

//TODO: run the renderer and game loop on two separate threads
void engine::start(jaw::properties* props) {
	assert(props != nullptr);

	timeGetDevCaps(&timerInfo, sizeof(timerInfo));
	timeBeginPeriod(timerInfo.wPeriodMin);
	auto _ = QueryPerformanceFrequency(&countsPerSecond);

	HWND hwnd = win::init(props);
	ValidateRect(hwnd, NULL);
	draw::init(props, hwnd);
	input::init();
	game::init();
	startPoint = lastFrame = getTimePoint();

	do {
		input::beginFrame();
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		game::loop();
		draw::prepareRender();
		draw::render();
		prelimit(props);
		ValidateRect(hwnd, NULL);
		draw::present();	// This will block until VBLANK if Vsync is on
		limiter(props);
	} while (running);

	input::deinit();
	draw::deinit();
	win::deinit();
	timeEndPeriod(timerInfo.wPeriodMin);
}

void engine::stop() {
	running = false;
}

jaw::nanoseconds engine::getUptime() {
	return getTimePoint() - startPoint;
}