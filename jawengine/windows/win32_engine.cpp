#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../JawEngine.h"
#include "win32_internal_draw.h"
#include "win32_internal_win.h"
#include "../common/internal_asset.h"
#include "../common/avx2.h"	//isAVX2
#include "../common/internal_utils.h"

#ifndef JAW_NSPRITE
#include "../common/internal_sprite.h"
#endif

#ifndef JAW_NSOUND
#include "../common/internal_sound.h"
#endif

#ifndef JAW_NINPUT
#include "../common/internal_input.h"
#include "../windows/win32_internal_dinput.h"
#endif

#ifndef JAW_NSTATE
#include "../common/internal_state.h"
#endif

#include <objbase.h>	//CoInitializeEx
#include <mmsystem.h>	//timer

static bool running;
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

static void prelimit(jaw::properties *props) {
	// Record how long the frame took to process before any kind of limiting
	props->logicFrametime = getTimePoint() - lastFrame;
}

static void limiter(jaw::properties *props) {
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

static bool isWINE() {
	static bool cached = false;
	static bool wine = false;

	if (!cached) {
		HMODULE ntdll = GetModuleHandleA("ntdll.dll");
		wine = ntdll && GetProcAddress(ntdll, "wine_get_version");
		cached = true;
	}
	return wine;
}

// TODO: update this routine to probe for AVX2 support by trying to execute an instruction
#include <iostream>
static void readCPU(jaw::properties *props) {
	bool avx2 = false;

	if (isWINE()) {
		// This assumes that any system running WINE supports AVX2
		// This will NOT always be the case, and will crash on systems that don't
		// WINE does not correctly support the CPUID flag and xgetbv checks that I do for Windows below
		avx2 = true;
		std::cerr << "Warning: Assuming AVX2 support under WINE. This may crash on CPUs without AVX2 extensions\n";
	}
	else {
		avx2 = isAVX2();
	}
#ifdef __AVX2__
	if (avx2) {
		props->cpuid.avx2 = true;
		return;
	}
	else {
		MessageBox(NULL, "Your CPU does not support AVX2", "Instruction Set Not Supported", MB_OK | MB_ICONWARNING);
		exit(1);
	}
#else
	props->cpuid.avx2 = avx2;
#endif
}

//TODO: run the renderer and game loop on two separate threads
void engine::start(jaw::properties *props, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop) {
	assert(props != nullptr);

	// Read CPU flags for instruction set extensions
	readCPU(props);

	// This is for single-threaded only
	auto hr_ = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

	timeGetDevCaps(&timerInfo, sizeof(timerInfo));
	timeBeginPeriod(timerInfo.wPeriodMin);
	auto b = QueryPerformanceFrequency(&countsPerSecond);

/*
*	Subsystem Initialization
*/

	HWND hwnd = win::init(props);
	ValidateRect(hwnd, NULL);
	draw::init(props, hwnd);
	asset::init();

	if (!util::init(props)) {
		MessageBox(NULL, "malloc failed to allocate memory\nIs the system out of RAM?", "malloc failure", MB_OK | MB_ICONWARNING);
		exit(1);
	}

#ifndef JAW_NSOUND
	sound::init();
#endif

#ifndef JAW_NINPUT
	input::init(hwnd);
#endif

#ifdef JAW_NSTATE
	if (initOnce) initOnce(props);
	if (init) init(props);
#else
	auto sid = state::create(props, initOnce, init, loop);
	assert(sid == 0);
	state::push(sid);
#endif

/*
*	Loop
*/

	startPoint = lastFrame = getTimePoint();
	running = true;
	do {
		util::beginFrame();
#ifndef JAW_NINPUT
		input::beginFrame(props);
		input::readGamepads();
#endif
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

#ifndef JAW_NSPRITE
		sprite::updateAll(props);
#endif

		util::updateTimers(props);

#ifdef JAW_NSTATE
		loop(props);
#else
		if (!state::loop(props)) {
			engine::stop();
			break;
		}
#endif

#ifndef JAW_NSPRITE
		sprite::drawAll(props);
#endif

		draw::prepareRender();
		draw::render();
		prelimit(props);
		ValidateRect(hwnd, NULL);
		draw::present();	// This will block until VBLANK if Vsync is on
		limiter(props);
	} while (running);

/*
*	Deinitialization and clean-up
*/

#ifndef JAW_NSPRITE
	sprite::clear();
	anim::clear();
#endif

#ifndef JAW_NINPUT
	input::deinit();
#endif

#ifndef JAW_NSTATE
	state::deinit();
#endif

#ifndef JAW_NSOUND
	sound::deinit();
#endif

	util::deinit();
	asset::deinit();
	draw::deinit();
	win::deinit(hwnd);
	timeEndPeriod(timerInfo.wPeriodMin);
	CoUninitialize();
	*props = {};
}

void engine::stop() {
	running = false;
}