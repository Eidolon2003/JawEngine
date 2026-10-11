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

#pragma once
#include <cstdint>
#include <type_traits>

constexpr float PI32 = 3.14159265f;

namespace jaw {
	// Provides useful information about the system at runtime
	struct Sysinfo {
		bool wine;
		bool avx2;
	};
	// The one and only global
	// Set before main is called by the engine's entry point
	extern Sysinfo sysinfo;

	/*
		COMMON TYPES
	*/
	typedef int64_t nanoseconds;
	constexpr jaw::nanoseconds millis(float m) { return (jaw::nanoseconds)(m * 1'000'000); }
	constexpr float to_millis(jaw::nanoseconds n) { return n / 1'000'000.f; }
	constexpr jaw::nanoseconds seconds(float s) { return (jaw::nanoseconds)(s * 1'000'000'000); }
	constexpr float to_seconds(jaw::nanoseconds n) { return n / 1'000'000'000.f; }

	constexpr uint32_t INVALID_ID = UINT32_MAX;

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
		constexpr vec2f operator*(const vec2f rhs) const { return { x * rhs.x, y * rhs.y }; }
		constexpr vec2f operator/(const float rhs) const { return { x / rhs, y / rhs }; }
		constexpr vec2f operator/(const vec2f rhs) const { return { x / rhs.x, y / rhs.y }; }

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
		constexpr vec2i(jaw::vec2f v) { x = (int16_t)v.x; y = (int16_t)v.y; }

		constexpr vec2i operator+(const vec2i rhs) const { return { (int16_t)(x + rhs.x), (int16_t)(y + rhs.y) }; }
		constexpr vec2i operator+(const int16_t rhs) const { return { (int16_t)(x + rhs), (int16_t)(y + rhs) }; }
		constexpr vec2i operator-(const vec2i rhs) const { return { (int16_t)(x - rhs.x), (int16_t)(y - rhs.y) }; }
		constexpr vec2i operator-(const int16_t rhs) const { return { (int16_t)(x - rhs), (int16_t)(y - rhs) }; }
		constexpr vec2i operator*(const int16_t rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator*(const float rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator*(const vec2i rhs) const { return { (int16_t)(x * rhs.x), (int16_t)(y * rhs.y) }; }
		constexpr vec2i operator/(const float rhs) const { return { (int16_t)(x / rhs), (int16_t)(y / rhs) }; }
		constexpr vec2i operator/(const vec2i rhs) const { return { (int16_t)(x / rhs.x), (int16_t)(y / rhs.y) }; }

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

/*
	PROPERTIES STRUCT
*/
	// This struct is the engine's general purpose context.
	// It defines properties during start up (desired size, title, etc),
	// it has runtime relevant information (mouse, uptime, frametime, etc),
	// and void *data provides a way to pass application specific global context
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

		// The commit size of util::frameAllocator in bytes
		size_t frameAllocatorBytes = 16<<20;

		// The commit size of state::stateAllocator in bytes
		size_t stateAllocatorBytes = 16<<20;

		//These are automatically populated by the system, read only
		vec2i winsize = vec2i();
		uint64_t framecount = 0;
		jaw::nanoseconds totalFrametime = 0;
		jaw::nanoseconds logicFrametime = 0;
		jaw::nanoseconds uptime = 0;

		//Convenience functions
		vec2i scaledSize() const {
			return size * scale;
		}
	};

/*
	DRAW TYPES
*/
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

		constexpr argb dark(argb c) { return (c & 0xFF000000) | ((c & 0x00FEFEFE) >> 1); }

		constexpr argb DARK_RED = dark(RED);
		constexpr argb DARK_GREEN = dark(GREEN);
		constexpr argb DARK_BLUE = dark(BLUE);
		constexpr argb GRAY = dark(WHITE);
		constexpr argb GREY = GRAY;
		constexpr argb DARK_GRAY = dark(GRAY);
		constexpr argb DARK_GREY = DARK_GRAY;
		constexpr argb DARK_CYAN = dark(CYAN);
		constexpr argb DARK_MAGENTA = dark(MAGENTA);
		constexpr argb DARK_YELLOW = dark(YELLOW);
	};

/*
	SOUND TYPES
*/
#ifndef JAW_NSOUND
	typedef uint32_t soundid;
#endif

/*
	INPUT TYPES
*/
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
		bool isDown;	// true for one frame if the key was pressed
		bool isHeld;	// continuously true if the key is held down
	};
	static_assert(std::is_trivial_v<key>);

	typedef uint32_t clickableid;
	typedef void (*clickfn)(jaw::clickableid, jaw::properties*);
	struct clickable {
		jaw::recti *rect;
		jaw::clickfn callback;
		jaw::mouseFlags condition;
		void *data;
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

/*
	STATE TYPES
*/
#ifndef JAW_NSTATE
	typedef uint32_t stateid;
#endif
	// These do not get removed with JAW_NSTATE
	// The engine always depends on them
	typedef void (*statefn)(jaw::properties*);
	struct stateFns {
		jaw::statefn initOnce;
		jaw::statefn init;
		jaw::statefn deinit;
		jaw::statefn loop;
	};
	static_assert(std::is_trivial_v<stateFns>);

/*
	CALLBACK TYPES
*/
#ifndef JAW_NCALLBACK
	typedef uint32_t callbackid;
	typedef void (*callbackfn)(jaw::callbackid, jaw::properties *);
	// Callbacks are configurable to work on either frame intervals or walltime intervals
	// It will try frame interval first, but if frameInterval is set to zero, it will fallback to time
	// If timeInterval is also zero, the callback will never fire.
	struct callback {
		callbackfn callback;
		uint64_t frameInterval;
		uint64_t _prevFrame;	// When in frame interval mode, marks the previous frame the callback was fired
		jaw::nanoseconds timeInterval;
		jaw::nanoseconds _prevTime;	// When in time interval mode, marks the previous time the callback was fired
		void *data;
	};
	static_assert(std::is_trivial_v<callback>);
#endif
}