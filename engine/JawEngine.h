#pragma once

#if (defined (_WIN32) || defined (_WIN64))
#define WINDOWS
#elif (defined (LINUX) || defined (__linux__))
#define LINUX
#endif

#include <cstdint>
#include <chrono>
#include <functional>
#include <algorithm>

namespace jaw {

	struct EngineProperties {
		bool showCMD;
	};

	struct AppProperties {
		double framerate;
	};

	class GraphicsInterface {

	};

	class SoundInterface {

	};

	class InputInterface {
	public:
		virtual std::pair<int, int> getMouseXY() = 0;
		virtual std::string getString() = 0;
	};

	class AppInterface;
	class EngineInterface {
	public:
		virtual void OpenWindow(AppInterface*, const AppProperties&) = 0;
		virtual void CloseWindow(AppInterface*) = 0;
		virtual void ShowCMD(bool) = 0;
	};

	class WindowInterface {
	public:
		virtual std::chrono::duration<double, std::milli> getFrametime() = 0;
		virtual std::chrono::duration<uint64_t, std::milli> getLifetime() = 0;
	};

	class AppInterface {
	public:
		GraphicsInterface* pGraphics = nullptr;
		SoundInterface* pSound = nullptr;
		InputInterface* pInput = nullptr;
		WindowInterface* pWindow = nullptr;
		EngineInterface* pEngine = nullptr;

		virtual void Loop() = 0;
	};

	void StartEngine(AppInterface*, const AppProperties&, const EngineProperties&);
	

#if defined WINDOWS
	//https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	constexpr unsigned char  BACK = 0x08;
	constexpr unsigned char  TAB = 0x09;
	constexpr unsigned char  CLEAR = 0x0C;
	constexpr unsigned char  ENTER = 0x0D;
	constexpr unsigned char  SHIFT = 0x10;
	constexpr unsigned char  CTRL = 0x11;
	constexpr unsigned char  ALT = 0x12;
	constexpr unsigned char  PAUSE = 0x13;
	constexpr unsigned char  CAPSLOCK = 0x14;
	constexpr unsigned char  ESC = 0x1B;
	constexpr unsigned char  SPACE = 0x20;
	constexpr unsigned char  PAGEUP = 0x21;
	constexpr unsigned char  PAGEDOWN = 0x22;
	constexpr unsigned char  END = 0x23;
	constexpr unsigned char  HOME = 0x24;
	constexpr unsigned char  LEFT = 0x25;
	constexpr unsigned char  UP = 0x26;
	constexpr unsigned char  RIGHT = 0x27;
	constexpr unsigned char  DOWN = 0x28;
	constexpr unsigned char  PRINTSCREEN = 0x2C;
	constexpr unsigned char  INS = 0x2D;
	constexpr unsigned char  DEL = 0x2E;

	constexpr unsigned char  ZERO = 0x30;
	constexpr unsigned char  ONE = 0x31;
	constexpr unsigned char  TWO = 0x32;
	constexpr unsigned char  THREE = 0x33;
	constexpr unsigned char  FOUR = 0x34;
	constexpr unsigned char  FIVE = 0x35;
	constexpr unsigned char  SIX = 0x36;
	constexpr unsigned char  SEVEN = 0x37;
	constexpr unsigned char  EIGHT = 0x38;
	constexpr unsigned char  NINE = 0x39;

	constexpr unsigned char  A = 0x41;
	constexpr unsigned char  B = 0x42;
	constexpr unsigned char  C = 0x43;
	constexpr unsigned char  D = 0x44;
	constexpr unsigned char  E = 0x45;
	constexpr unsigned char  F = 0x46;
	constexpr unsigned char  G = 0x47;
	constexpr unsigned char  H = 0x48;
	constexpr unsigned char  I = 0x49;
	constexpr unsigned char  J = 0x4A;
	constexpr unsigned char  K = 0x4B;
	constexpr unsigned char  L = 0x4C;
	constexpr unsigned char  M = 0x4D;
	constexpr unsigned char  N = 0x4E;
	constexpr unsigned char  O = 0x4F;
	constexpr unsigned char  P = 0x50;
	constexpr unsigned char  Q = 0x51;
	constexpr unsigned char  R = 0x52;
	constexpr unsigned char  S = 0x53;
	constexpr unsigned char  T = 0x54;
	constexpr unsigned char  U = 0x55;
	constexpr unsigned char  V = 0x56;
	constexpr unsigned char  W = 0x57;
	constexpr unsigned char  X = 0x58;
	constexpr unsigned char  Y = 0x59;
	constexpr unsigned char  Z = 0x5A;

	constexpr unsigned char  LWIN = 0x5B;
	constexpr unsigned char  RWIN = 0x5C;

	constexpr unsigned char  NUMPAD0 = 0x60;
	constexpr unsigned char  NUMPAD1 = 0x61;
	constexpr unsigned char  NUMPAD2 = 0x62;
	constexpr unsigned char  NUMPAD3 = 0x63;
	constexpr unsigned char  NUMPAD4 = 0x64;
	constexpr unsigned char  NUMPAD5 = 0x65;
	constexpr unsigned char  NUMPAD6 = 0x66;
	constexpr unsigned char  NUMPAD7 = 0x67;
	constexpr unsigned char  NUMPAD8 = 0x68;
	constexpr unsigned char  NUMPAD9 = 0x69;

	constexpr unsigned char  MULTIPLY = 0x6A;
	constexpr unsigned char  ADD = 0x6B;
	constexpr unsigned char  SUBTRACT = 0x6D;
	constexpr unsigned char  DECIMAL = 0x6E;
	constexpr unsigned char  DIVIDE = 0x6F;

	constexpr unsigned char  F1 = 0x70;
	constexpr unsigned char  F2 = 0x71;
	constexpr unsigned char  F3 = 0x72;
	constexpr unsigned char  F4 = 0x73;
	constexpr unsigned char  F5 = 0x74;
	constexpr unsigned char  F6 = 0x75;
	constexpr unsigned char  F7 = 0x76;
	constexpr unsigned char  F8 = 0x77;
	constexpr unsigned char  F9 = 0x78;
	constexpr unsigned char  F10 = 0x79;
	constexpr unsigned char  F11 = 0x7A;
	constexpr unsigned char  F12 = 0x7B;
	constexpr unsigned char  F13 = 0x7C;
	constexpr unsigned char  F14 = 0x7D;
	constexpr unsigned char  F15 = 0x7E;
	constexpr unsigned char  F16 = 0x7F;
	constexpr unsigned char  F17 = 0x80;
	constexpr unsigned char  F18 = 0x81;
	constexpr unsigned char  F19 = 0x82;
	constexpr unsigned char  F20 = 0x83;
	constexpr unsigned char  F21 = 0x84;
	constexpr unsigned char  F22 = 0x85;
	constexpr unsigned char  F23 = 0x86;
	constexpr unsigned char  F24 = 0x87;

	constexpr unsigned char  NUMLOCK = 0x90;
	constexpr unsigned char  SCROLLLOCK = 0x91;

	constexpr unsigned char  LSHIFT = 0xA0;
	constexpr unsigned char  RSHIFT = 0xA1;
	constexpr unsigned char  LCTRL = 0xA2;
	constexpr unsigned char  RCTRL = 0xA3;
#endif
};