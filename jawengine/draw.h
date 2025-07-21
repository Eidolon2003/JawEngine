#pragma once
#include "structs.h"

namespace draw {
	typedef uint32_t bmpid;
	typedef uint32_t fontid;
	typedef uint32_t argb;

	constexpr size_t MAX_DRAW_SIZE = 30;
	constexpr size_t MAX_QUEUE_SIZE = 1024 * 1024;
	constexpr size_t MAX_NUM_FONTS = 1024;
	
	namespace color {
		constexpr argb RED = 0xFFFF0000;
		constexpr argb GREEN = 0xFF00FF00;
		constexpr argb BLUE = 0xFF0000FF;
		constexpr argb WHITE = 0xFFFFFFFF;
		constexpr argb BLACK = 0xFF000000;
	};

/*	-----------------------------------
	Drawing Primitives
*/
	struct lineOptions {
		jaw::vec2i p1, p2;
		argb color;
		uint32_t width;
	};
	static_assert(sizeof(lineOptions) <= MAX_DRAW_SIZE);
	bool line(lineOptions*, uint8_t z);

	struct rectOptions {
		jaw::recti rect;
		argb color;
	};
	static_assert(sizeof(rectOptions) <= MAX_DRAW_SIZE);
	bool rect(rectOptions*, uint8_t z);

	//Note: The string this is pointing to must exist past the end of the frame
	struct strOptions {
		jaw::recti rect;
		argb color;
		fontid font = 0;
		const char* str;
	};
	static_assert(sizeof(strOptions) <= MAX_DRAW_SIZE);
	bool str(strOptions*, uint8_t z);

/*	-----------------------------------
	Assets
*/
	struct fontOptions {
		const char* name = "Consolas";
		float size = 16.f;
		bool italic = false;
		bool bold = false;
		enum { CENTER, LEFT, RIGHT } align = LEFT;
	};
	fontid newFont(fontOptions*);

/*	-----------------------------------
	Lower level API below. The functions above are recommended for general use
*/
	enum type : uint8_t { 
		LINE, RECT, STR
	};

	struct alignas(32) drawCall {
		type t;
		uint8_t z;
		uint8_t data[MAX_DRAW_SIZE];
	};
	static_assert(sizeof(drawCall) == 32);
	drawCall makeDraw(type t, uint8_t z, void* opt);
	bool enqueue(drawCall*);
	bool enqueueMany(drawCall*, size_t);
}