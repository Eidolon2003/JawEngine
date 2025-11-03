#include "../JawEngine.h"
#include "win32_internal_dinput.h"

#include <dinput.h>

struct DIGamepad {
	IDirectInputDevice8A *dev;
	GUID guid;
	DIJOYSTATE2 state;
	WORD vid, pid;
};

static jaw::gamepad external[input::MAX_GAMEPADS]{};
static DIGamepad internal[input::MAX_GAMEPADS];
static unsigned numPlayers;

static IDirectInput8A *di;

void input::init(HWND hwnd) {
	clear();
	numPlayers = 0;

	HINSTANCE inst = GetModuleHandleA(NULL);
	DirectInput8Create(inst, DIRECTINPUT_VERSION, IID_IDirectInput8A, (LPVOID*)&di, NULL);
	findNewGamepads();
}

void input::deinit() {
	input::clear();
	numPlayers = 0;
}

static void readSony(jaw::SonyGamepad &sony, DIJOYSTATE2 &joy) {
	sony.square.isDown = joy.rgbButtons[0] >= 0x80;
	sony.x.isDown = joy.rgbButtons[1] >= 0x80;
	sony.circle.isDown = joy.rgbButtons[2] >= 0x80;
	sony.triangle.isDown = joy.rgbButtons[3] >= 0x80;
	sony.l1.isDown = joy.rgbButtons[4] >= 0x80;
	sony.r1.isDown = joy.rgbButtons[5] >= 0x80;
	sony.select.isDown = joy.rgbButtons[8] >= 0x80;
	sony.start.isDown = joy.rgbButtons[9] >= 0x80;
	sony.l3.isDown = joy.rgbButtons[10] >= 0x80;
	sony.r3.isDown = joy.rgbButtons[11] >= 0x80;
	sony.ps.isDown = joy.rgbButtons[12] >= 0x80;
	sony.pad.isDown = joy.rgbButtons[13] >= 0x80;

	sony.r = jaw::vec2f(
		(float)(joy.lZ - 32767) / 32768.f,
		(float)(joy.lRz - 32767) / 32768.f
	);
	sony.l = jaw::vec2f(
		(float)(joy.lX - 32767) / 32768.f,
		(float)(joy.lY - 32767) / 32768.f
	);

	sony.l2 = joy.lRx / 65535.f;
	sony.r2 = joy.lRy / 65535.f;
}

void input::readGamepads() {
	for (unsigned i = 0; i < numPlayers; i++) {
		DIGamepad &g = internal[i];

		if (!g.dev) continue;

		HRESULT hr = g.dev->Poll();
		if (FAILED(hr)) {
			hr = g.dev->Acquire();
			if (FAILED(hr)) {
				switch (hr) {
				case DIERR_INPUTLOST:
					continue;

				case DIERR_UNPLUGGED:
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
	bool newFound;
	di->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumCallback, (LPVOID)&newFound, DIEDFL_ATTACHEDONLY);
	if (newFound) readGamepads();
	return newFound;
}

const jaw::gamepad *input::getGamepad(unsigned index) {
	if (index >= numPlayers) return nullptr;
	if (internal[index].dev == nullptr) return nullptr;
	return external + index;
}