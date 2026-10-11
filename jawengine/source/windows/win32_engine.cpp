/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.1
 * https://github.com/Eidolon2003/JawEngine
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../../JawEngine.h"
#include "win32_internal_draw.h"
#include "win32_internal_win.h"
#include "../common/internal_utils.h"
#include "../../headers/utils.h"

#ifndef JAW_NASSET
#include "../common/internal_asset.h"
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

#ifndef JAW_NCALLBACK
#include "../common/internal_callback.h"
#endif

#include <objbase.h>	//CoInitializeEx

static bool running;
static jaw::nanoseconds startPoint, lastFrame, thisFrame;

static void prelimit(jaw::properties *props) {
	// Record how long the frame took to process before any kind of limiting
	props->logicFrametime = util::getTimePoint() - lastFrame;
}

static void limiter(jaw::properties *props) {
	props->framecount++;
	thisFrame = util::getTimePoint();
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
	thisFrame = util::accurateSleep(targetFrametime - thisFrametime, thisFrame);
	thisFrametime = thisFrame - lastFrame;
	assert(thisFrametime >= targetFrametime);

end:
	props->totalFrametime = thisFrametime;
	props->uptime += thisFrametime;
	lastFrame = thisFrame;
	return;
}

//TODO: run the renderer and game loop on two separate threads
void engine::start(jaw::properties *props, const jaw::stateFns &fns) {
	if (props == nullptr) return;

	// This is for single-threaded only
	(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

/*
*	Subsystem Initialization
*/

	HWND hwnd = win::init(props);
	ValidateRect(hwnd, NULL);
	draw::init(props, hwnd);

	if (!util::init(props)) {
		MessageBox(NULL, "malloc failed to allocate memory\nIs the system out of RAM?", "malloc failure", MB_OK | MB_ICONWARNING);
		exit(1);
	}

#ifndef JAW_NASSET
	asset::init();
#endif

#ifndef JAW_NCALLBACK
	callback::init();
#endif

#ifndef JAW_NSOUND
	sound::init();
#endif

#ifndef JAW_NINPUT
	input::init(hwnd);
#endif

#ifdef JAW_NSTATE
	if (fns.initOnce) fns.initOnce(props);
	if (fns.init) fns.init(props);
#else
	// Pushing the initial state before creating it is intentional
	// This allows for the game to push inside its initial set up.
	// We want our zero state to be at the bottom of the stack
	state::init(props);
	state::push(0);
	auto sid = state::create(props, fns);
	if (sid != 0) return;
#endif

/*
*	Loop
*/

	startPoint = lastFrame = util::getTimePoint();
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

		util::updateTimers(props);

#ifdef JAW_NSTATE
		if (fns.loop) fns.loop(props);
#else
		if (!state::loop(props)) {
			running = false;
			break;
		}
#endif

#ifndef JAW_NCALLBACK
		callback::loop(props);
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
#ifndef JAW_NINPUT
	input::deinit();
#endif

#ifdef JAW_NSTATE
	if (fns.deinit) fns.deinit(props);
#else
	state::deinit(props);
#endif

#ifndef JAW_NSOUND
	sound::deinit();
#endif

#ifndef JAW_NASSET
	asset::deinit();
#endif

	util::deinit();
	draw::deinit();
	win::deinit(hwnd);
	CoUninitialize();
	*props = {};
}

void engine::stop() {
	running = false;
}