#pragma once
#include <cstdint>

namespace jaw {
	typedef int64_t nanoseconds;
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
	};

	//TODO: more convenient op overloads for these structs
	struct vec2f {
		float x, y;
		constexpr vec2f(float x = 0, float y = 0) { this->x = x; this->y = y; }

		constexpr vec2f operator+(const vec2f rhs) const { return { x + rhs.x, y + rhs.y }; }
		constexpr vec2f operator+(const float rhs) const { return { x + rhs, y + rhs }; }
		constexpr vec2f operator-(const vec2f rhs) const { return { x - rhs.x, y - rhs.y }; }
		constexpr vec2f operator*(const float rhs) const { return { x * rhs, y * rhs }; }
		constexpr vec2f operator/(const float rhs) const { return { x / rhs, y / rhs }; }
	};

	struct vec2i {
		int16_t x, y;
		constexpr vec2i(int16_t x = 0, int16_t y = 0) { this->x = x; this->y = y; }

		constexpr vec2i operator+(const vec2i rhs) const { return { x + rhs.x, y + rhs.y }; }
		constexpr vec2i operator+(const int16_t rhs) const { return { x + rhs, y + rhs }; }
		constexpr vec2i operator-(const vec2i rhs) const { return { x - rhs.x, y - rhs.y }; }
		constexpr vec2i operator*(const int16_t rhs) const { return { x * rhs, y * rhs }; }
		constexpr vec2i operator*(const float rhs) const { return { (int16_t)(x * rhs), (int16_t)(y * rhs) }; }
		constexpr vec2i operator/(const int16_t rhs) const { return { x / rhs, y / rhs }; }
		constexpr vec2i operator/(const float rhs) const { return { (int16_t)(x / rhs), (int16_t)(y / rhs) }; }

		constexpr bool operator<(const vec2i rhs) const { return (x < rhs.x) && (y < rhs.y); }
		constexpr bool operator>=(const int16_t rhs) const { return (x >= rhs) && (y >= rhs); }
	};

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

	struct mouse {
		jaw::vec2i pos;
		int32_t wheelDelta;
		jaw::mouseFlags flags;
	};
	
	struct properties {
		const char* title = " ";
		vec2i size = vec2i(640, 480);		// The logical size of the window
		float scale = 1.f;					// Integer scaling values use nearest neighbor
		float targetFramerate = 0;			// <=0 means VSync
		bool enableKeyRepeat = false;
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
			// if size evently divides the screen size, nearest neighbor is used
			FULLSCREEN_STRETCHED
		} mode = WINDOWED;

		//These are automatically populated by the system
		vec2i winsize = vec2i();
		uint64_t framecount = 0;
		jaw::nanoseconds totalFrametime = 0;
		jaw::nanoseconds logicFrametime = 0;

		//Convenience functions
		vec2i scaledSize() const {
			return size * scale;
		}
	};
}