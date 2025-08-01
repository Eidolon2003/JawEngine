#pragma once
#include "types.h"

namespace draw {
	constexpr size_t MAX_DRAW_SIZE = 30;
	constexpr size_t MAX_QUEUE_SIZE = 1024*1024;
	constexpr size_t MAX_NUM_FONTS = 1024;
	constexpr size_t MAX_NUM_BMPS = 1024;
	
	void setBackgroundColor(jaw::argb);

/*	-----------------------------------
	Drawing Primitives
*/
	struct lineOptions {
		jaw::vec2i p1, p2;
		jaw::argb color;
		uint32_t width;
	};
	static_assert(sizeof(lineOptions) <= MAX_DRAW_SIZE);
	bool line(const lineOptions*, uint8_t z);
	inline bool line(const lineOptions& o, uint8_t z) { return line(&o, z); }

	struct rectOptions {
		jaw::recti rect;
		jaw::argb color;
	};
	static_assert(sizeof(rectOptions) <= MAX_DRAW_SIZE);
	bool rect(const rectOptions*, uint8_t z);
	inline bool rect(const rectOptions& o, uint8_t z) { return rect(&o, z); }

	//Note: The string this is pointing to must exist past the end of the frame
	struct strOptions {
		jaw::recti rect;
		jaw::argb color;
		jaw::fontid font = 0;
		const char* str;
	};
	static_assert(sizeof(strOptions) <= MAX_DRAW_SIZE);
	bool str(const strOptions*, uint8_t z);
	inline bool str(const strOptions& o, uint8_t z) { return str(&o, z); }

	struct bmpOptions {
		jaw::bmpid bmp;
		jaw::recti src;
		jaw::recti dest;
	};
	static_assert(sizeof(bmpOptions) <= MAX_DRAW_SIZE);
	bool bmp(const bmpOptions*, uint8_t z);
	inline bool bmp(const bmpOptions& o, uint8_t z) { return bmp(&o, z); }

	struct ellipseOptions {
		jaw::ellipse ellipse;
		jaw::argb color;
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
	jaw::fontid newFont(const fontOptions*);
	inline jaw::fontid newFont(const fontOptions& o) { return newFont(&o); }

	jaw::bmpid createBmp(jaw::vec2i size);

	//Copies pixels into a bitmap. numPixels MUST equal the width x height of the bitmap
	//This routine is slow compared to others in the API, so better to call as little as needed
	bool writeBmp(jaw::bmpid, const jaw::argb* pixels, size_t numPixels);
	inline bool writeBmp(jaw::bmpid b, const jaw::argb* pixels, jaw::vec2i dim) {
		return writeBmp(b, pixels, (dim.x * dim.y));
	}

/*	-----------------------------------
	Lower level API below. The functions above are recommended for general use
*/
	enum type : uint8_t { 
		LINE,
		RECT,
		STR,
		BMP,
		ELLIPSE,
		NUM_TYPES
	};

	constexpr size_t typeSizes[] = {
		sizeof(lineOptions),
		sizeof(rectOptions),
		sizeof(strOptions),
		sizeof(bmpOptions),
		sizeof(ellipseOptions),
	};
	static_assert(sizeof(typeSizes) / sizeof(size_t) == type::NUM_TYPES);

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

	// Creates a BMP in the same way as createBmp, but that is usable with renderToBmp
	jaw::bmpid createRenderableBmp(jaw::vec2i size);

	// Immediately render to bitmap target for later use
	bool renderToBmp(const drawCall&, jaw::bmpid);
}