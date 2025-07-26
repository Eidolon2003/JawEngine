#pragma once
#include "types.h"

namespace draw {
	typedef uint32_t bmpid;
	typedef uint32_t fontid;
	typedef uint32_t argb;

	constexpr size_t MAX_DRAW_SIZE = 30;
	constexpr size_t MAX_QUEUE_SIZE = 1024*1024;
	constexpr size_t MAX_NUM_FONTS = 1024;
	constexpr size_t MAX_NUM_BMPS = 1024;
	
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

	void setBackgroundColor(argb);

/*	-----------------------------------
	Drawing Primitives
*/
	struct lineOptions {
		jaw::vec2i p1, p2;
		argb color;
		uint32_t width;
	};
	static_assert(sizeof(lineOptions) <= MAX_DRAW_SIZE);
	bool line(const lineOptions*, uint8_t z);
	inline bool line(const lineOptions& o, uint8_t z) { return line(&o, z); }

	struct rectOptions {
		jaw::recti rect;
		argb color;
	};
	static_assert(sizeof(rectOptions) <= MAX_DRAW_SIZE);
	bool rect(const rectOptions*, uint8_t z);
	inline bool rect(const rectOptions& o, uint8_t z) { return rect(&o, z); }

	//Note: The string this is pointing to must exist past the end of the frame
	struct strOptions {
		jaw::recti rect;
		argb color;
		fontid font = 0;
		const char* str;
	};
	static_assert(sizeof(strOptions) <= MAX_DRAW_SIZE);
	bool str(const strOptions*, uint8_t z);
	inline bool str(const strOptions& o, uint8_t z) { return str(&o, z); }

	struct bmpOptions {
		bmpid bmp;
		jaw::recti src;
		jaw::recti dest;
	};
	static_assert(sizeof(bmpOptions) <= MAX_DRAW_SIZE);
	bool bmp(const bmpOptions*, uint8_t z);
	inline bool bmp(const bmpOptions& o, uint8_t z) { return bmp(&o, z); }

	struct ellipseOptions {
		jaw::ellipse ellipse;
		argb color;
	};
	static_assert(sizeof(ellipseOptions) <= MAX_DRAW_SIZE);
	bool ellipse(const ellipseOptions*, uint8_t z);
	inline bool ellipse(const ellipseOptions& o, uint8_t z) { return ellipse(&o, z); }

	//TODO: more primitives?

/*	-----------------------------------
	Assets
*/
	struct fontOptions {
		const char* name = "Courier New";
		float size = 10.f;
		bool italic = false;
		bool bold = false;
		enum { CENTER, LEFT, RIGHT } align = LEFT;
	};
	fontid newFont(const fontOptions*);
	inline fontid newFont(const fontOptions& o) { return newFont(&o); }


	bmpid loadBmp(const char* filename);
	bmpid createBmp(jaw::vec2i size);

	//Copies a number of pixels equal to the bmp's size into it
	//These don't currently support alpha transparency, alpha channel ignored
	bool writeBmp(bmpid, const argb* pixels);

/*	-----------------------------------
	Lower level API below. The functions above are recommended for general use
*/
	enum type : uint8_t { 
		LINE,
		RECT,
		STR,
		BMP,
		ELLIPSE,
	};

	constexpr size_t typeSizes[] = {
		sizeof(lineOptions),
		sizeof(rectOptions),
		sizeof(strOptions),
		sizeof(bmpOptions),
		sizeof(ellipseOptions),
	};

	struct alignas(32) drawCall {
		type t;
		uint8_t z;
		uint8_t data[MAX_DRAW_SIZE];
	};
	static_assert(sizeof(drawCall) == 32);
	drawCall makeDraw(type t, uint8_t z, const void* opt);
	bool enqueue(const drawCall*);
	inline bool enqueue(const drawCall& d) { return enqueue(&d); }
	bool enqueueMany(const drawCall*, size_t);
}