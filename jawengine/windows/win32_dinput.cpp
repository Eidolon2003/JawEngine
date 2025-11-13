#include "../JawEngine.h"
#include "win32_internal_dinput.h"

#include <dinput.h>

#ifndef NDEBUG
#include <iostream>
#endif

struct DIGamepad {
	IDirectInputDevice8A *dev;
	GUID guid;
	DIJOYSTATE2 state;
	WORD vid, pid;
};
static_assert(std::is_trivial_v<DIGamepad>);

static jaw::gamepad external[input::MAX_GAMEPADS];
static DIGamepad internal[input::MAX_GAMEPADS];
static unsigned numPlayers;

static IDirectInput8A *di;

void input::init(HWND hwnd) {
#ifndef NDEBUG
	std::cout << "Debug: DirectInput8 init ";
#endif

	clear();
	numPlayers = 0;

	HINSTANCE inst = GetModuleHandleA(NULL);
	HRESULT hr = DirectInput8Create(inst, DIRECTINPUT_VERSION, IID_IDirectInput8A, (LPVOID*)&di, NULL);

#ifndef NDEBUG
	std::cout << (SUCCEEDED(hr) ? "succeeded\n" : "failed\n");
#endif

	findNewGamepads();
}

void input::deinit() {
	input::clear();
	numPlayers = 0;
}

static void readSony(jaw::SonyGamepad &sony, DIJOYSTATE2 &joy) {
	auto setKey = [](jaw::key &k, bool b) {
		k.isDown = b && !k.isHeld;
		k.isHeld = b;
	};

	auto setIndexed = [joy, &setKey](jaw::key &k, size_t i) {
		bool b = joy.rgbButtons[i] >= 0x80;
		setKey(k, b);
	};

	setIndexed(sony.square,		0);
	setIndexed(sony.x,			1);
	setIndexed(sony.circle,		2);
	setIndexed(sony.triangle,	3);
	setIndexed(sony.l1,			4);
	setIndexed(sony.r1,			5);
	setIndexed(sony.l2,			6);
	setIndexed(sony.r2,			7);
	setIndexed(sony.select,		8);
	setIndexed(sony.start,		9);
	setIndexed(sony.l3,			10);
	setIndexed(sony.r3,			11);
	setIndexed(sony.ps,			12);
	setIndexed(sony.pad,		13);

	auto dpad = (signed)joy.rgdwPOV[0];
	setKey(sony.up,		dpad > 27000 || (dpad < 9000 && dpad > -1));
	setKey(sony.right,	dpad > 0 && dpad < 18000);
	setKey(sony.down,	dpad > 9000 && dpad < 27000);
	setKey(sony.left,	dpad > 18000);

	sony.r = jaw::vec2f(
		(float)(joy.lZ - 32767) / 32768.f,
		(float)(joy.lRz - 32767) / 32768.f
	);
	sony.l = jaw::vec2f(
		(float)(joy.lX - 32767) / 32768.f,
		(float)(joy.lY - 32767) / 32768.f
	);

	sony.l2a = joy.lRx / 65535.f;
	sony.r2a = joy.lRy / 65535.f;
}

// Unfortunately because gamepads are polled only once per frame,
// key.isDown is not true if a button is pressed and release within the same frame, unlike the kbd.
// I believe this can be improved using DI buffered input
void input::readGamepads() {
	for (unsigned i = 0; i < numPlayers; i++) {
		DIGamepad &g = internal[i];

		if (!g.dev) continue;

		HRESULT hr = g.dev->Poll();
		if (FAILED(hr)) {
			hr = g.dev->Acquire();
			if (FAILED(hr)) {
				if (hr == DIERR_INPUTLOST) continue;
				else if (hr == DIERR_UNPLUGGED) {
#ifndef NDEBUG
					std::cout << "Debug: Controller slot " << i << " unplugged\n";
#endif
					g.dev->Release();
					g = {};
					continue;
				}
			}
		}

		g.dev->GetDeviceState(sizeof(g.state), &g.state);

		switch (external[i].type) {
		case jaw::gamepad::type::SONY:
			readSony(external[i].sony, internal[i].state);
			break;

		case jaw::gamepad::type::UNKNOWN:
		default:
			break;
		}
	}
}

unsigned input::numGamepads() {
	return numPlayers;
}

static BOOL EnumCallback(LPCDIDEVICEINSTANCEA lpddi, LPVOID ptr) {
	bool *newFound = (bool*)ptr;
	*newFound = false;

	// Check if we're out of space
	if (numPlayers >= input::MAX_GAMEPADS) return DIENUM_CONTINUE;

	// Check if this controller has already been set up
	GUID instance = lpddi->guidInstance;
	for (DIGamepad &g : internal) {
		if (g.guid == instance) return DIENUM_CONTINUE;
	}

	// Once here this is a new controller
#ifndef NDEBUG
	std::cout << "Debug: New controller found: " << lpddi->tszProductName << std::endl;
#endif

	unsigned index = 0;
	for (; index < numPlayers; index++) {
		if (internal[index].dev == nullptr) break;
	}
	DIGamepad &g = internal[index] = {
		.dev = nullptr,
		.guid = instance
	};

	HRESULT hr = di->CreateDevice(instance, &g.dev, NULL);
	if (FAILED(hr)) goto fail;

	hr = g.dev->SetCooperativeLevel(GetConsoleWindow(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(hr)) goto fail1;

	hr = g.dev->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr)) goto fail1;

	g.vid = LOWORD(lpddi->guidProduct.Data1);
	g.pid = HIWORD(lpddi->guidProduct.Data1);

	if (g.vid == 0x054C) {
		external[index].type = jaw::gamepad::type::SONY;
	}
	else {
		external[index].type = jaw::gamepad::type::UNKNOWN;
	}

	*newFound = true;
	if (index == numPlayers) numPlayers++;
	return DIENUM_CONTINUE;

fail1:
	g.dev->Release();
fail:
	g = {};
	*newFound = false;
	return DIENUM_CONTINUE;
}
bool input::findNewGamepads() {
#ifndef NDEBUG
	std::cout << "Debug: input::findNewGamepads()\n";
#endif

	bool newFound;
	HRESULT hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumCallback, (LPVOID)&newFound, DIEDFL_ATTACHEDONLY);

#ifndef NDEBUG
	std::cout << "Debug: DIEnumDevices " << (SUCCEEDED(hr) ? "succeeded\n" : "failed\n");
#endif

	if (newFound) readGamepads();
	return newFound;
}

const jaw::gamepad *input::getGamepad(unsigned index) {
	if (index >= numPlayers) return nullptr;
	if (internal[index].dev == nullptr) return nullptr;
	return external + index;
}
