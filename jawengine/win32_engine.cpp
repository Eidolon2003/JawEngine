#include "JawEngine.h"
#include "win32_internal_draw.h"
#include "win32_internal_win.h"

static bool running = true;
static LARGE_INTEGER countsPerSecond;
static TIMECAPS timerInfo;
jaw::nanoseconds startPoint, lastFrame, thisFrame;

jaw::nanoseconds getTimePoint() {
	LARGE_INTEGER timePoint;
	auto _ = QueryPerformanceCounter(&timePoint);
	return timePoint.QuadPart * (1'000'000'000ULL / countsPerSecond.QuadPart);
}

jaw::nanoseconds accurateSleep(jaw::nanoseconds time, jaw::nanoseconds startPoint) {
	int msTimerAccuracy = timerInfo.wPeriodMin;
	int msSleepTime = (((time / 1'000'000LL) / msTimerAccuracy) - 1) * msTimerAccuracy;
	if (msSleepTime > 0) Sleep((DWORD)msSleepTime);
	jaw::nanoseconds retTime;
	while ((retTime = getTimePoint()) - startPoint < time);
	return retTime;
}

void limiter(jaw::properties* props) {
	props->framecount++;
	thisFrame = getTimePoint();
	jaw::nanoseconds frametime = thisFrame - lastFrame;

	if (props->targetFramerate <= 0) {
		props->logicFrametime = props->totalFrametime = frametime;
		lastFrame = thisFrame;
		return;
	}

	jaw::nanoseconds targetFrametime = (jaw::nanoseconds)(1'000'000'000.0 / props->targetFramerate);

	if (frametime >= targetFrametime * 10) {
		startPoint += frametime - targetFrametime;
		props->logicFrametime = props->totalFrametime = targetFrametime;
		lastFrame = thisFrame;
		return;
	}

	if (frametime >= targetFrametime) {
		props->logicFrametime = props->totalFrametime = frametime;
		lastFrame = thisFrame;
		return;
	}

	thisFrame = accurateSleep(targetFrametime - frametime, thisFrame);
	props->logicFrametime = frametime;
	props->totalFrametime = thisFrame - lastFrame;
	lastFrame = thisFrame;
}

//TODO: run the renderer and game loop on two separate threads
void engine::start(jaw::properties* props) {
	timeGetDevCaps(&timerInfo, sizeof(timerInfo));
	timeBeginPeriod(timerInfo.wPeriodMin);
	auto _ = QueryPerformanceFrequency(&countsPerSecond);

	HWND hwnd = win::init(props);
	draw::init(props, hwnd);
	game::init();
	startPoint = lastFrame = getTimePoint();

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
		limiter(props);
	} while (running);

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