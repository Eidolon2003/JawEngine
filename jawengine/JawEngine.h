#ifndef WINDOWS
#ifndef LINUX

#if (defined _WIN32 || defined _WIN64)
#define WINDOWS
#define NOMINMAX	//Suppress Microsoft's #defined min and max macros
#elif (defined LINUX || defined __linux__)
#define LINUX
#endif

#include <cstdint>
#include <string>
#include <chrono>
#include <functional>
#include <algorithm>
#include <memory>
#include <set>

namespace jaw {

	struct Point {
		int16_t x, y;

		Point() { x = y = 0; }
		Point(int16_t x, int16_t y) { this->x = x; this->y = y; }
		
		bool operator==(Point rhs) const { return x == rhs.x && y == rhs.y; }
		bool operator<(Point rhs) const { if (x == rhs.x) return y < rhs.y; else return x < rhs.x; }
		Point operator+(Point rhs) const { return Point(x + rhs.x, y + rhs.y); }
		Point operator+(int16_t rhs) const { return Point(x + rhs, y + rhs); }
		Point operator-(Point rhs) const { return Point(x - rhs.x, y - rhs.y); }
		Point operator-(int16_t rhs) const { return Point(x - rhs, y - rhs); }
		Point operator*(Point rhs) const { return Point(x * rhs.x, y * rhs.y); }
		Point operator*(int16_t rhs) const { return Point(x * rhs, y * rhs); }
		Point operator/(Point rhs) const { return Point(x / rhs.x, y / rhs.y); }
		Point operator/(int16_t rhs) const { return Point(x / rhs, y / rhs); }
	};

	struct Rect {
		Point tl, br;

		Rect() { tl = Point(); br = Point(); }
		Rect(Point tl, Point br) { this->tl = tl; this->br = br; }
		Rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2) { tl = Point(x1, y1); br = Point(x2, y2); }
		
		bool operator==(const Rect& rhs) const { return tl == rhs.tl && br == rhs.br; }
	};

	struct EngineProperties {
		bool showCMD = false;
		std::wstring locale = L"en-us";
	};

	struct AppProperties {
		const char* title = " ";
		jaw::Point size = jaw::Point(640, 480);
		enum { WINDOWED, WINDOWED_FULLSCREEN } mode = WINDOWED;
		float scale = 1.f;
		float framerate = 60;
		bool enableKeyRepeat = false;
		bool enableSubpixelTextRendering = false;
		uint8_t layerCount = 8;
		uint8_t backgroundCount = 1;
	};

	struct Font {
		std::wstring name = L"Consolas";
		float size = 12.0f;
		bool italic = false;
		bool bold = false;
		enum { CENTER, LEFT, RIGHT } align = LEFT;
	};

	class Bitmap {
	public:
		virtual std::string getName() const = 0;
		virtual Point getSize() const = 0;
	};
	
	class AppInterface;
	class Sprite {
	public:
		float x, y, dx, dy;
		float scale;
		Bitmap* bmp;
		Rect src;
		uint8_t layer, frame;
		bool hidden;
		std::chrono::duration<float, std::milli> lifetime;
		std::chrono::duration<float, std::milli> animationTiming;
		std::chrono::duration<float, std::milli> animationCounter;

		Sprite();
		virtual ~Sprite() {}

		//These two functions are called by the engine automatically in this order
		//Returns true if the sprite should be deleted
		virtual bool Update(AppInterface*);
		//Called if the sprite wasn't deleted
		virtual void Draw(AppInterface*);

		Point getPoint() const;
		void setPoint(Point pos);
		Point getSize() const;
	};

	class GraphicsInterface {
	public:
		virtual Bitmap* LoadBmp(std::string filename) = 0;

		virtual bool DrawBmp(std::string filename, Point dest, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) = 0;
		virtual bool DrawBmp(std::string filename, Rect dest, uint8_t layer, float alpha = 1.f, bool interpolation = false) = 0;
		virtual bool DrawBmp(const Bitmap* bmp, Point dest, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) = 0;
		virtual bool DrawBmp(const Bitmap* bmp, Rect dest, uint8_t layer, float alpha = 1.f, bool interpolation = false) = 0;

		virtual bool DrawPartialBmp(std::string filename, Rect dest, Rect src, uint8_t layer, float alpha = 1.f, bool interpolation = false) = 0;
		virtual bool DrawPartialBmp(std::string filename, Point dest, Rect src, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) = 0;
		virtual bool DrawPartialBmp(const Bitmap* bmp, Rect dest, Rect src, uint8_t layer, float alpha = 1.f, bool interpolation = false) = 0;
		virtual bool DrawPartialBmp(const Bitmap* bmp, Point dest, Rect src, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) = 0;

		//Default sprite drawing routine
		//This will not call an overloaded Sprite::Draw method
		virtual bool DrawSprite(const Sprite* sprite) = 0;
		virtual bool DrawSprite(const Sprite& sprite) = 0;

		virtual bool LoadFont(const Font&) = 0;
		virtual bool DrawString(std::wstring str, Rect dest, uint8_t layer, const Font& font = Font(), uint32_t color = 0xFFFFFF, float alpha = 1.f) = 0;

		virtual void setBackgroundColor(uint32_t color) = 0;
		virtual void ClearLayer(uint8_t layer, uint32_t color = 0x000000, float alpha = 1.f) = 0;
		virtual void FillRect(Rect dest, uint32_t color, uint8_t layer, float alpha = 1.f) = 0;
		virtual void DrawLine(Point start, Point end, uint32_t width, uint32_t color, uint8_t layer, float alpha = 1.f) = 0;
	};

	class SoundInterface {
	public:
		virtual bool Load(std::string) = 0;
		virtual bool Play(std::string) = 0;
		virtual bool Loop(std::string) = 0;
		virtual bool Stop(std::string) = 0;
		virtual void StopAll() = 0;
	};

	class InputInterface {
	public:
		struct Mouse {
			union {
				struct {
					char lmb : 1;
					char rmb : 1;
					char shift : 1;
					char ctrl : 1;
					char mmb : 1;
					char xmb1 : 1;
					char xmb2 : 1;
				};
				uint8_t flags;
			};
			jaw::Point pos;
			int16_t wheel;
		};

		virtual Mouse getMouse() const = 0;
		virtual void ResetWheel() = 0;
		virtual std::wstring getString() const = 0;
		virtual bool isKeyPressed(uint8_t) const = 0;
		virtual void BindKeyDown(uint8_t, const std::function<void()>&) = 0;
		virtual void BindKeyUp(uint8_t, const std::function<void()>&) = 0;
		virtual void BindClickDown(const std::function<void(Mouse)>&) = 0;
		virtual void BindClickUp(const std::function<void(Mouse)>&) = 0;
	};

	class EngineInterface {
	public:
		virtual void OpenWindow(AppInterface*, const AppProperties&) = 0;
		virtual void CloseWindow(AppInterface*) = 0;
		virtual void ShowCMD(bool) = 0;
		virtual std::wstring getLocale() const = 0;
	};

	class WindowInterface {
	protected:
		std::set<std::shared_ptr<Sprite>> sprites;
	public:
		virtual std::chrono::duration<double, std::milli> getFrametime() const = 0;
		virtual std::chrono::duration<uint64_t, std::milli> getLifetime() const = 0;
		virtual const AppProperties& getProperties() const = 0;

		//Turn ownership of your sprite over to the engine
		//You retain access via a weak_ptr
		template <typename SPR>
		std::weak_ptr<SPR> RegisterSprite(SPR* spr) {
			static_assert(std::is_base_of<Sprite, SPR>::value,
				"Your sprite should inherit from jaw::Sprite");

			auto shared = std::shared_ptr<SPR>(spr);
			std::weak_ptr<SPR> weak = shared;
			sprites.insert(shared);
			return weak;
		}

		//Take ownership of sprite back from the engine
		template <typename SPR>
		SPR* DeregisterSprite(std::weak_ptr<SPR> spr) {
			static_assert(std::is_base_of<Sprite, SPR>::value, 
				"Your sprite should inherit from jaw::Sprite");

			if (spr.expired()) return nullptr;
			std::shared_ptr<SPR> new_shared = spr.lock();
			sprites.erase(new_shared);
			return new_shared.get();
		}
	};

	class AppInterface {
	public:
		GraphicsInterface* graphics = nullptr;
		SoundInterface* sound = nullptr;
		InputInterface* input = nullptr;
		WindowInterface* window = nullptr;
		EngineInterface* engine = nullptr;

		virtual ~AppInterface() {}
		virtual void Init() = 0;
		virtual void Loop() = 0;
	};

	void StartEngine(AppInterface*, const AppProperties&, const EngineProperties&);
	

#if defined WINDOWS
	//https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	constexpr uint8_t BACK = 0x08;
	constexpr uint8_t TAB = 0x09;
	constexpr uint8_t CLEAR = 0x0C;
	constexpr uint8_t ENTER = 0x0D;
	constexpr uint8_t SHIFT = 0x10;
	constexpr uint8_t CTRL = 0x11;
	constexpr uint8_t ALT = 0x12;
	constexpr uint8_t PAUSE = 0x13;
	constexpr uint8_t CAPSLOCK = 0x14;
	constexpr uint8_t ESC = 0x1B;
	constexpr uint8_t SPACE = 0x20;
	constexpr uint8_t PAGEUP = 0x21;
	constexpr uint8_t PAGEDOWN = 0x22;
	constexpr uint8_t END = 0x23;
	constexpr uint8_t HOME = 0x24;
	constexpr uint8_t LEFT = 0x25;
	constexpr uint8_t UP = 0x26;
	constexpr uint8_t RIGHT = 0x27;
	constexpr uint8_t DOWN = 0x28;
	constexpr uint8_t PRINTSCREEN = 0x2C;
	constexpr uint8_t INS = 0x2D;
	constexpr uint8_t DEL = 0x2E;

	constexpr uint8_t ZERO = 0x30;
	constexpr uint8_t ONE = 0x31;
	constexpr uint8_t TWO = 0x32;
	constexpr uint8_t THREE = 0x33;
	constexpr uint8_t FOUR = 0x34;
	constexpr uint8_t FIVE = 0x35;
	constexpr uint8_t SIX = 0x36;
	constexpr uint8_t SEVEN = 0x37;
	constexpr uint8_t EIGHT = 0x38;
	constexpr uint8_t NINE = 0x39;

	constexpr uint8_t A = 0x41;
	constexpr uint8_t B = 0x42;
	constexpr uint8_t C = 0x43;
	constexpr uint8_t D = 0x44;
	constexpr uint8_t E = 0x45;
	constexpr uint8_t F = 0x46;
	constexpr uint8_t G = 0x47;
	constexpr uint8_t H = 0x48;
	constexpr uint8_t I = 0x49;
	constexpr uint8_t J = 0x4A;
	constexpr uint8_t K = 0x4B;
	constexpr uint8_t L = 0x4C;
	constexpr uint8_t M = 0x4D;
	constexpr uint8_t N = 0x4E;
	constexpr uint8_t O = 0x4F;
	constexpr uint8_t P = 0x50;
	constexpr uint8_t Q = 0x51;
	constexpr uint8_t R = 0x52;
	constexpr uint8_t S = 0x53;
	constexpr uint8_t T = 0x54;
	constexpr uint8_t U = 0x55;
	constexpr uint8_t V = 0x56;
	constexpr uint8_t W = 0x57;
	constexpr uint8_t X = 0x58;
	constexpr uint8_t Y = 0x59;
	constexpr uint8_t Z = 0x5A;

	constexpr uint8_t LWIN = 0x5B;
	constexpr uint8_t RWIN = 0x5C;

	constexpr uint8_t NUMPAD0 = 0x60;
	constexpr uint8_t NUMPAD1 = 0x61;
	constexpr uint8_t NUMPAD2 = 0x62;
	constexpr uint8_t NUMPAD3 = 0x63;
	constexpr uint8_t NUMPAD4 = 0x64;
	constexpr uint8_t NUMPAD5 = 0x65;
	constexpr uint8_t NUMPAD6 = 0x66;
	constexpr uint8_t NUMPAD7 = 0x67;
	constexpr uint8_t NUMPAD8 = 0x68;
	constexpr uint8_t NUMPAD9 = 0x69;

	constexpr uint8_t MULTIPLY = 0x6A;
	constexpr uint8_t ADD = 0x6B;
	constexpr uint8_t SUBTRACT = 0x6D;
	constexpr uint8_t DECIMAL = 0x6E;
	constexpr uint8_t DIVIDE = 0x6F;

	constexpr uint8_t F1 = 0x70;
	constexpr uint8_t F2 = 0x71;
	constexpr uint8_t F3 = 0x72;
	constexpr uint8_t F4 = 0x73;
	constexpr uint8_t F5 = 0x74;
	constexpr uint8_t F6 = 0x75;
	constexpr uint8_t F7 = 0x76;
	constexpr uint8_t F8 = 0x77;
	constexpr uint8_t F9 = 0x78;
	constexpr uint8_t F10 = 0x79;
	constexpr uint8_t F11 = 0x7A;
	constexpr uint8_t F12 = 0x7B;
	constexpr uint8_t F13 = 0x7C;
	constexpr uint8_t F14 = 0x7D;
	constexpr uint8_t F15 = 0x7E;
	constexpr uint8_t F16 = 0x7F;
	constexpr uint8_t F17 = 0x80;
	constexpr uint8_t F18 = 0x81;
	constexpr uint8_t F19 = 0x82;
	constexpr uint8_t F20 = 0x83;
	constexpr uint8_t F21 = 0x84;
	constexpr uint8_t F22 = 0x85;
	constexpr uint8_t F23 = 0x86;
	constexpr uint8_t F24 = 0x87;

	constexpr uint8_t NUMLOCK = 0x90;
	constexpr uint8_t SCROLLLOCK = 0x91;

	constexpr uint8_t LSHIFT = 0xA0;
	constexpr uint8_t RSHIFT = 0xA1;
	constexpr uint8_t LCTRL = 0xA2;
	constexpr uint8_t RCTRL = 0xA3;
#endif
};

#endif
#endif