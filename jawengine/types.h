#pragma once
#include <cstdint>
#include <type_traits>

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

		constexpr argb DARK_RED = 0xFF770000;
		constexpr argb DARK_GREEN = 0xFF007700;
		constexpr argb DARK_BLUE = 0xFF000077;
		constexpr argb GRAY = 0xFF777777;
		constexpr argb GREY = GRAY;
		constexpr argb DARK_GRAY = 0xFF333333;
		constexpr argb DARK_GREY = DARK_GRAY;
		constexpr argb DARK_CYAN = 0xFF007777;
		constexpr argb DARK_MAGENTA = 0xFF770077;
		constexpr argb DARK_YELLOW = 0xFF777700;
	};

	//TODO: more convenient op overloads for these structs
	struct vec2i;
	struct vec2f {
		float x, y;
		vec2f() = default;
		constexpr vec2f(float x, float y) { this->x = x; this->y = y; }
		constexpr vec2f(jaw::vec2i);

		constexpr vec2f operator+(const vec2f rhs) const { return { x + rhs.x, y + rhs.y }; }
		constexpr vec2f operator+(const float rhs) const { return { x + rhs, y + rhs }; }
		constexpr vec2f operator-(const vec2f rhs) const { return { x - rhs.x, y - rhs.y }; }
		constexpr vec2f operator-(const float rhs) const { return { x - rhs, y - rhs }; }
		constexpr vec2f operator*(const float rhs) const { return { x * rhs, y * rhs }; }
		constexpr vec2f operator/(const float rhs) const { return { x / rhs, y / rhs }; }

		constexpr bool operator<(const vec2f rhs) const { return (x < rhs.x) && (y < rhs.y); }
		constexpr bool operator>=(const vec2f rhs) const { return (x >= rhs.x) && (y >= rhs.y); }
		constexpr bool operator==(const vec2f rhs) const { return (x == rhs.x) && (y == rhs.y); }
		constexpr bool operator!=(const vec2f rhs) const { return !operator==(rhs); }

		constexpr float product() const { return x*y; }
	};
	static_assert(std::is_trivial_v<vec2f>);

	struct vec2i {
		int16_t x, y;
		vec2i() = default;
		constexpr vec2i(int16_t x, int16_t y) { this->x = x; this->y = y; }
		constexpr vec2i(jaw::vec2f v) { x = (uint16_t)v.x; y = (uint16_t)v.y; }

		constexpr vec2i operator+(const vec2i rhs) const { return { (int16_t)(x + rhs.x), (int16_t)(y + rhs.y) }; }
		constexpr vec2i operator+(const int16_t rhs) const { return { (int16_t)(x + rhs), (int16_t)(y + rhs) }; }
		constexpr vec2i operator-(const vec2i rhs) const { return { (int16_t)(x - rhs.x), (int16_t)(y - rhs.y) }; }
		constexpr vec2i operator-(const int16_t rhs) const { return { (int16_t)(x - rhs), (int16_t)(y - rhs) }; }
		constexpr vec2i operator*(const int16_t rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator*(const float rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator/(const float rhs) const { return { (int16_t)(x / rhs), (int16_t)(y / rhs) }; }

		constexpr bool operator<(const vec2i rhs) const { return (x < rhs.x) && (y < rhs.y); }
		constexpr bool operator>=(const vec2i rhs) const { return (x >= rhs.x) && (y >= rhs.y); }
		constexpr bool operator==(const vec2i rhs) const { return (x == rhs.x) && (y == rhs.y); }
		constexpr bool operator!=(const vec2i rhs) const { return !operator==(rhs); }

		constexpr int32_t product() const { return x*y; }
	};
	inline constexpr jaw::vec2f::vec2f(jaw::vec2i v) { x = (float)v.x; y = (float)v.y; }
	static_assert(std::is_trivial_v<vec2i>);

	struct rectf {
		vec2f tl, br;
		rectf() = default;
		constexpr rectf(float tlx, float tly, float brx, float bry) {
			tl.x = tlx; tl.y = tly; br.x = brx; br.y = bry;
		}
		constexpr rectf(vec2f tl, vec2f br) { this->tl = tl; this->br = br; }

		constexpr float width() const { return br.x - tl.x; }
		constexpr float height() const { return br.y - tl.y; }
		constexpr vec2f tr() const { return vec2f(br.x, tl.y); }
		constexpr vec2f bl() const { return vec2f(tl.x, br.y); }

		constexpr bool contains(const jaw::vec2f pt) const { return pt >= tl && pt < br; }
		constexpr bool collides(const jaw::rectf r) const {
			return r.contains(tl) ||
				r.contains(br) ||
				r.contains(tr()) ||
				r.contains(bl());
		}
	};
	static_assert(std::is_trivial_v<rectf>);

	struct recti {
		vec2i tl, br;
		recti() = default;
		constexpr recti(int16_t tlx, int16_t tly, int16_t brx, int16_t bry) {
			tl.x = tlx; tl.y = tly; br.x = brx; br.y = bry;
		}
		constexpr recti(vec2i tl, vec2i br) { this->tl = tl; this->br = br; }

		constexpr int16_t width() const { return br.x - tl.x; }
		constexpr int16_t height() const { return br.y - tl.y; }
		constexpr vec2i tr() const { return vec2i(br.x, tl.y); }
		constexpr vec2i bl() const { return vec2i(tl.x, br.y); }

		constexpr bool contains(const jaw::vec2i pt) const { return pt >= tl && pt < br; }
		constexpr bool collides(const jaw::recti r) const {
			return r.contains(tl) ||
				r.contains(br) ||
				r.contains(tr()) ||
				r.contains(bl());
		}
	};
	static_assert(std::is_trivial_v<recti>);

	struct ellipse {
		vec2i center;
		vec2i radii;
		ellipse() = default;
		constexpr ellipse(vec2i c, vec2i r) {
			center = c; radii = r;
		}
	};
	static_assert(std::is_trivial_v<ellipse>);

#ifndef JAW_NINPUT
	union mouseFlags {
		uint8_t all;
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
	static_assert(std::is_trivial_v<mouseFlags>);

	struct mouse {
		jaw::vec2i pos;
		int32_t wheelDelta;
		jaw::mouseFlags flags;
		jaw::mouseFlags prevFlags;
	};
	static_assert(std::is_trivial_v<mouse>);

	struct key {
		bool isDown;
		bool isHeld;
	};
	static_assert(std::is_trivial_v<key>);
#endif

	struct cpuflags {
		bool avx2;
	};
	static_assert(std::is_trivial_v<cpuflags>);
	
	struct properties {
		const char *title = " ";
		vec2i size = vec2i(640, 480);		// The logical size of the window
		float scale = 1.f;					// Integer scaling values use nearest neighbor
		float targetFramerate = 0;			// <=0 means VSync
		int monitorIndex = -1;				// Which monitor the window should open on
											// Negative means primary, high values are capped
		bool enableSubpixelTextRendering = false;
		bool enablePerPrimitiveAA = false;

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

#ifdef NDEBUG
		bool showCMD = false;
#else
		bool showCMD = true;
#endif

		// This may be used for any sort of game data that needs to be passed around
		void *data = nullptr;

		// The size of the temporary allocator arena in bytes
		size_t tempallocBytes = 1<<20;

		//These are automatically populated by the system, read only
		vec2i winsize = vec2i();
		uint64_t framecount = 0;
		jaw::nanoseconds totalFrametime = 0;
		jaw::nanoseconds logicFrametime = 0;
		jaw::nanoseconds uptime = 0;
		jaw::cpuflags cpuid{};

#ifndef JAW_NINPUT
		jaw::mouse mouse{};
#endif

		//Convenience functions
		vec2i scaledSize() const {
			return size * scale;
		}
	};

#ifndef JAW_NSOUND
	typedef uint32_t soundid;
#endif

#ifndef JAW_NSPRITE
	typedef uint32_t sprid;
	typedef uint32_t animstateid;
	typedef uint32_t animdefid;

	struct animation {
		unsigned startFrame;
		unsigned endFrame;
		unsigned row;
		jaw::nanoseconds frameInterval;
		bool loop;
	};
	static_assert(std::is_trivial_v<animation>);

	// The engine will automatically handle the following:
	// - Update pos using vel
	// - Update age
	// - Handle animation
	struct sprite {
		// Pixel coordinate and velocity in pixels per second
		jaw::vec2f pos, vel;
		uint8_t z;
		bool mirrorX;
		bool mirrorY;

		// The id of this sprite's bitmap
		jaw::bmpid bmp;

		// The size in pixels of a single animation frame,
		// or the sprite itself if not animated
		jaw::vec2i frameSize;

		// Age is updated by the system
		jaw::nanoseconds age;

		// Optional: if the sprite is animated
		jaw::animstateid animState;

		//Convenience functions
		constexpr jaw::recti rect() const { return jaw::recti(pos, jaw::vec2i(pos) + frameSize); }
	};
	static_assert(std::is_trivial_v<sprite>);

	typedef void (*sprfn)(jaw::sprid, jaw::properties*);
#endif

	typedef void (*statefn)(jaw::properties*);
	typedef jaw::recti(*rectfn)(jaw::properties*);

#ifndef JAW_NINPUT
	typedef uint32_t clickableid;
	struct clickable {
		jaw::rectfn getRect;
		jaw::statefn callback;
		jaw::mouseFlags condition;
	};
	static_assert(std::is_trivial_v<clickable>);

	struct SonyGamepad {
		jaw::key x, square, circle, triangle;
		jaw::key up, down, left, right;
		jaw::key select, start;
		jaw::key r1, l1, r2, l2, r3, l3;
		float r2a, l2a;
		jaw::vec2f r, l;
		jaw::key ps, pad;
	};
	static_assert(std::is_trivial_v<SonyGamepad>);

	struct gamepad {
		enum class type { SONY, UNKNOWN } type;
		union {
			SonyGamepad sony;
		};
	};
	static_assert(std::is_trivial_v<gamepad>);
#endif

#ifndef JAW_NSTATE
	typedef uint32_t stateid;
#endif
}