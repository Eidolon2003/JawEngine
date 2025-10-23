#pragma once
#include <cstdint>

constexpr float PI32 = 3.14159265f;

namespace jaw {
	typedef int64_t nanoseconds;

	constexpr jaw::nanoseconds millis(float m) { return (jaw::nanoseconds)(m * 1'000'000); }
	constexpr float to_millis(jaw::nanoseconds n) { return n / 1'000'000.f; }
	constexpr jaw::nanoseconds seconds(float s) { return (jaw::nanoseconds)(s * 1'000'000'000); }
	constexpr float to_seconds(jaw::nanoseconds n) { return n / 1'000'000'000.f; }

	constexpr uint32_t INVALID_ID = UINT32_MAX;
	typedef uint32_t bmpid;
	typedef uint32_t fontid;
	typedef uint32_t stateid;
	typedef uint32_t sprid;
	typedef uint32_t animstateid;
	typedef uint32_t animdefid;
	typedef uint32_t clickableid;
	typedef uint32_t soundid;

	typedef uint32_t argb;
	namespace color {
		constexpr argb RED = 0xFFFF0000;
		constexpr argb GREEN = 0xFF00FF00;
		constexpr argb BLUE = 0xFF0000FF;
		constexpr argb WHITE = 0xFFFFFFFF;
		constexpr argb BLACK = 0xFF000000;
		constexpr argb CYAN = 0xFF00FFFF;
		constexpr argb MAGENTA = 0xFFFF00FF;
		constexpr argb YELLOW = 0xFFFFFF00;
	};

	//TODO: more convenient op overloads for these structs
	struct vec2i;
	struct vec2f {
		float x, y;
		constexpr vec2f(float x = 0, float y = 0) { this->x = x; this->y = y; }
		constexpr vec2f(jaw::vec2i);

		constexpr vec2f operator+(const vec2f rhs) const { return { x + rhs.x, y + rhs.y }; }
		constexpr vec2f operator+(const float rhs) const { return { x + rhs, y + rhs }; }
		constexpr vec2f operator-(const vec2f rhs) const { return { x - rhs.x, y - rhs.y }; }
		constexpr vec2f operator*(const float rhs) const { return { x * rhs, y * rhs }; }
		constexpr vec2f operator/(const float rhs) const { return { x / rhs, y / rhs }; }
	};

	struct vec2i {
		int16_t x, y;
		constexpr vec2i(int16_t x = 0, int16_t y = 0) { this->x = x; this->y = y; }
		constexpr vec2i(jaw::vec2f v) { x = (uint16_t)v.x; y = (uint16_t)v.y; }

		constexpr vec2i operator+(const vec2i rhs) const { return { (int16_t)(x + rhs.x), (int16_t)(y + rhs.y) }; }
		constexpr vec2i operator+(const int16_t rhs) const { return { (int16_t)(x + rhs), (int16_t)(y + rhs) }; }
		constexpr vec2i operator-(const vec2i rhs) const { return { (int16_t)(x - rhs.x), (int16_t)(y - rhs.y) }; }
		constexpr vec2i operator*(const int16_t rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator*(const float rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator/(const float rhs) const { return { (int16_t)(x / rhs), (int16_t)(y / rhs) }; }

		constexpr bool operator<(const vec2i rhs) const { return (x < rhs.x) && (y < rhs.y); }
		constexpr bool operator>=(const vec2i rhs) const { return (x >= rhs.x) && (y >= rhs.y); }
	};
	inline constexpr jaw::vec2f::vec2f(jaw::vec2i v) { x = (float)v.x; y = (float)v.y; }

	struct rectf {
		vec2f tl, br;
		constexpr rectf(float tlx = 0, float tly = 0, float brx = 0, float bry = 0) {
			tl.x = tlx; tl.y = tly; br.x = brx; br.y = bry;
		}
		constexpr rectf(vec2f tl, vec2f br) { this->tl = tl; this->br = br; }
	};

	struct recti {
		vec2i tl, br;
		constexpr recti(int16_t tlx = 0, int16_t tly = 0, int16_t brx = 0, int16_t bry = 0) {
			tl.x = tlx; tl.y = tly; br.x = brx; br.y = bry;
		}
		constexpr recti(vec2i tl, vec2i br) { this->tl = tl; this->br = br; }
	};

	struct ellipse {
		vec2i center;
		vec2i radii;
		constexpr ellipse(vec2i c = vec2i(), vec2i r = vec2i()) {
			center = c; radii = r;
		}
	};

	union mouseFlags {
		uint8_t all {};
		struct {
			char lmb : 1;
			char rmb : 1;
			char shift : 1;
			char ctrl : 1;
			char mmb : 1;
			char xmb1 : 1;
			char xmb2 : 1;
		};
	};

	struct mouse {
		jaw::vec2i pos;
		int32_t wheelDelta {};
		jaw::mouseFlags flags {};
		jaw::mouseFlags prevFlags {};
	};

	struct key {
		bool isDown;
		bool isHeld;
	};

	struct cpuflags {
		bool avx2;
	};
	
	struct properties {
		const char *title = " ";
		vec2i size = vec2i(640, 480);		// The logical size of the window
		float scale = 1.f;					// Integer scaling values use nearest neighbor
		float targetFramerate = 0;			// <=0 means VSync
		int monitorIndex = -1;				// Which monitor the window should open on
											// Negative means primary, high values are capped
		bool enableSubpixelTextRendering = false;
		bool enablePerPrimitiveAA = false;
		bool showCMD = false;
		enum { 
			// Drawable window size is size * scale
			WINDOWED,

			// A window of size (size * scale) is centered in the borderless fullscreen window
			// Zero values for either size dimension will be filled in with (screensize / scale)
			// A zero scale value will be filled in with the value required to fill the screen
			FULLSCREEN_CENTERED,

			// Same as FULLSCREEN_CENTERED, but only integer scale factors are used
			FULLSCREEN_CENTERED_INTEGER,

			// A window of size "size" is stretched to fill the borderless fullscreen window
			// Zero values for either size dimension will be filled in with the screen size
			// The user-defined scale value is unused
			// if size evenly divides the screen size, nearest neighbor is used
			FULLSCREEN_STRETCHED
		} mode = WINDOWED;

		// This may be used for any sort of game data that needs to be passed around
		void *data = nullptr;

		//These are automatically populated by the system, read only
		vec2i winsize = vec2i();
		uint64_t framecount = 0;
		jaw::nanoseconds totalFrametime = 0;
		jaw::nanoseconds logicFrametime = 0;
		jaw::nanoseconds uptime = 0;
		jaw::mouse mouse{};
		jaw::cpuflags cpuid;

		//Convenience functions
		vec2i scaledSize() const {
			return size * scale;
		}
	};

	struct animation {
		unsigned startFrame = 0;
		unsigned endFrame = 0;
		unsigned row = 0;
		jaw::nanoseconds frameInterval = jaw::millis(100);
		bool loop = true;
	};

	// The engine will automatically handle the following:
	// - Update pos using vel
	// - Update age
	// - Handle animation
	struct sprite {
		// Pixel coordinate and velocity in pixels per second
		jaw::vec2f pos{}, vel{};
		uint8_t z = 0;
		bool mirrorX = false;
		bool mirrorY = false;

		// The id of this sprite's bitmap
		jaw::bmpid bmp = INVALID_ID;

		// The size in pixels of a single animation frame,
		// or the sprite itself if not animated
		jaw::vec2i frameSize{};

		// Age is updated by the system
		jaw::nanoseconds age = 0;

		// Optional: if the sprite is animated
		jaw::animstateid animState = INVALID_ID;

		//Convenience functions
		constexpr jaw::recti rect() const { return jaw::recti(pos, jaw::vec2i(pos) + frameSize); }
	};

	typedef void (*statefn)(jaw::properties*);
	typedef void (*sprfn)(jaw::sprid, jaw::properties*);
	typedef jaw::recti(*rectfn)(jaw::properties*);

	struct clickable {
		jaw::rectfn getRect = nullptr;
		jaw::statefn callback = nullptr;
		jaw::mouseFlags condition{};
	};
}