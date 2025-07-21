#pragma once
#include <chrono>
using namespace std::chrono;

namespace jaw {
	struct vec2f {
		float x, y;
		vec2f(float x = 0, float y = 0) { this->x = x; this->y = y; }
	};

	struct vec2i {
		int16_t x, y;
		vec2i(int16_t x = 0, int16_t y = 0) { this->x = x; this->y = y; }
	};

	struct rectf {
		vec2f tl, br;
		rectf(float tlx = 0, float tly = 0, float brx = 0, float bry = 0) {
			tl.x = tlx; tl.y = tly; br.x = brx; br.y = bry;
		}
		rectf(vec2f tl, vec2f br) { this->tl = tl; this->br = br; }
	};

	struct recti {
		vec2i tl, br;
		recti(int16_t tlx = 0, int16_t tly = 0, int16_t brx = 0, int16_t bry = 0) {
			tl.x = tlx; tl.y = tly; br.x = brx; br.y = bry;
		}
		recti(vec2i tl, vec2i br) { this->tl = tl; this->br = br; }
	};

	struct properties {
		const char* title = " ";
		vec2i size = vec2i(640, 480);
		float scale = 1.f;
		float framerate = 60.f;
		bool enableKeyRepeat = false;
		bool enableSubpixelTextRendering = false;
		bool showCMD = false;
		enum { WINDOWED, WINDOWED_FULLSCREEN } mode = WINDOWED;

		//These are automatically populated by the system
		vec2i winsize = vec2i();
		duration<uint64_t, std::nano> uptime {0};
		time_point<high_resolution_clock, nanoseconds> lastframe = high_resolution_clock::now();
		duration<uint64_t, std::nano> frametime {0};
		uint64_t framecount = 0;

		//Convenience functions
		vec2i scaledSize() {
			return {
				(int16_t)(size.x * scale),
				(int16_t)(size.y * scale) 
			};
		}

		float getFramerate() {
			return 1'000'000'000.f / frametime.count();
		}
	};
}