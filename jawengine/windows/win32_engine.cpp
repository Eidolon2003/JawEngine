#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../JawEngine.h"
#include "win32_internal_draw.h"
#include "win32_internal_win.h"
#include "../common/internal_input.h"
#include "../common/internal_asset.h"
#include "../common/internal_state.h"
#include "../common/internal_sprite.h"
#include "../common/internal_sound.h"

#include <objbase.h>	//CoInitializeEx
#include <mmsystem.h>	//timer

static bool running = true;
static LARGE_INTEGER countsPerSecond;
static TIMECAPS timerInfo;
static jaw::nanoseconds startPoint, lastFrame, thisFrame;

static jaw::nanoseconds getTimePoint() {
	LARGE_INTEGER timePoint;
	auto _ = QueryPerformanceCounter(&timePoint);
	return timePoint.QuadPart * (1'000'000'000ULL / countsPerSecond.QuadPart);
}

static jaw::nanoseconds accurateSleep(jaw::nanoseconds time, jaw::nanoseconds startPoint) {
	int msTimerAccuracy = timerInfo.wPeriodMin;
	int msSleepTime = (int)(((time / 1'000'000LL) / msTimerAccuracy) - 1) * msTimerAccuracy;
	if (msSleepTime > 0) Sleep((DWORD)msSleepTime);
	// time remaining to wait is less than 2x the timer accuracy
	// tried going for 1x timer accuracy, but it made frame pacing less consistent
	assert((time - (getTimePoint() - startPoint)) < (timerInfo.wPeriodMin * 2'000'000));
	jaw::nanoseconds retTime;
	while ((retTime = getTimePoint()) - startPoint < time);
	return retTime;
}

static void prelimit(jaw::properties* props) {
	// Record how long the frame took to process before any kind of limiting
	props->logicFrametime = getTimePoint() - lastFrame;
}

static void limiter(jaw::properties* props) {
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
		thisFrametime = 0;
		goto end;
	}

	if (props->targetFramerate <= 0 || thisFrametime >= targetFrametime) {
		// either VSync is enabled, or we don't need to sleep
		goto end;
	}

	// Here we know we need to sleep for some time to hit the target framerate
	thisFrame = accurateSleep(targetFrametime - thisFrametime, thisFrame);
	thisFrametime = thisFrame - lastFrame;
	assert(thisFrametime >= targetFrametime);

end:
	props->totalFrametime = thisFrametime;
	props->uptime += thisFrametime;
	lastFrame = thisFrame;
	return;
}

//TODO: run the renderer and game loop on two separate threads
void engine::start(jaw::properties* props, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop) {
	assert(props != nullptr);

	// This is for single-threaded only
	auto hr_ = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

	timeGetDevCaps(&timerInfo, sizeof(timerInfo));
	timeBeginPeriod(timerInfo.wPeriodMin);
	auto b = QueryPerformanceFrequency(&countsPerSecond);

	HWND hwnd = win::init(props);
	ValidateRect(hwnd, NULL);
	draw::init(props, hwnd);
	sound::init();
	asset::init();
	startPoint = lastFrame = getTimePoint();

	auto sid = state::create(props, initOnce, init, loop);
	assert(sid == 0);
	state::push(sid);

	do {
		input::beginFrame(props);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		sprite::tick(props);

		if (!state::loop(props)) {
			engine::stop();
			break;
		}

		draw::prepareRender();
		draw::render();
		prelimit(props);
		ValidateRect(hwnd, NULL);
		draw::present();	// This will block until VBLANK if Vsync is on
		limiter(props);
	} while (running);

	asset::deinit();
	sound::deinit();
	draw::deinit();
	win::deinit();
	timeEndPeriod(timerInfo.wPeriodMin);
	CoUninitialize();
}

void engine::stop() {
	running = false;
}