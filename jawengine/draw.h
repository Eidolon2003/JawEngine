#pragma once
#include "types.h"

namespace draw {
	constexpr size_t MAX_DRAW_SIZE = 30;
	constexpr size_t MAX_QUEUE_SIZE = 65536;
	constexpr size_t MAX_NUM_FONTS = 1024;
	constexpr size_t MAX_NUM_BMPS = 1024;
	
	void setBackgroundColor(jaw::argb);

/*	-----------------------------------
	Drawing Primitives
*/
	struct line {
		jaw::vec2i p1, p2;
		jaw::argb color;
		uint32_t width;
		float angle = 0;
	};
	static_assert(sizeof(line) <= MAX_DRAW_SIZE);

	struct rect {
		jaw::recti rect;
		jaw::argb color;
		float angle = 0;
	};
	static_assert(sizeof(rect) <= MAX_DRAW_SIZE);

	#pragma pack(push, 4)
	// Note: The string this is pointing to must exist past the end of the frame.
	// Memory allocated by tempalloc is safe to use here
	struct str {
		jaw::recti rect;
		const char *str;
		jaw::argb color;
		jaw::fontid font = 0;
		float angle = 0;
	};
	#pragma pack(pop)
	static_assert(sizeof(str) <= MAX_DRAW_SIZE);

	struct bmp {
		jaw::bmpid bmp;
		jaw::recti src;
		jaw::recti dest;
		float angle = 0;
		bool mirrorX = false;
		bool mirrorY = false;
	};
	static_assert(sizeof(bmp) <= MAX_DRAW_SIZE);

	struct ellipse {
		jaw::ellipse ellipse;
		jaw::argb color;
		float angle = 0;
	};
	static_assert(sizeof(ellipse) <= MAX_DRAW_SIZE);

	enum class type : uint8_t {
		LINE,
		RECT,
		STR,
		BMP,
		ELLIPSE,
		NUM_TYPES
	};

	struct alignas(32) drawCall {
		type t;
		uint8_t z;
		union {
			line line;
			rect rect;
			str str;
			bmp bmp;
			ellipse ellipse;
		};
	};
	static_assert(sizeof(drawCall) == 32);

	// Make a draw call structrue from a primitive's options
	template <typename T>
	drawCall make(const T&, uint8_t z);
	template<>
	drawCall make<line>(const line&, uint8_t z);
	template<>
	drawCall make<rect>(const rect&, uint8_t z);
	template<>
	drawCall make<str>(const str&, uint8_t z);
	template<>
	drawCall make<bmp>(const bmp&, uint8_t z);
	template<>
	drawCall make<ellipse>(const ellipse&, uint8_t z);

	// Draw a primitive to the screen
	template <typename T>
	bool enqueue(const T&, uint8_t);
	template<>
	bool enqueue<line>(const line&, uint8_t z);
	template<>
	bool enqueue<rect>(const rect&, uint8_t z);
	template<>
	bool enqueue<str>(const str&, uint8_t z);
	template<>
	bool enqueue<bmp>(const bmp&, uint8_t z);
	template<>
	bool enqueue<ellipse>(const ellipse&, uint8_t z);

	// Copy an array of draw calls into the render queue
	bool enqueueMany(const drawCall*, size_t);

/*	-----------------------------------
	Assets
*/
	struct font {
		const char *name = "Courier New";
		float size = 10.f;
		bool italic = false;
		bool bold = false;
		enum { CENTER, LEFT, RIGHT } align = LEFT;
	};
	jaw::fontid newFont(const font*);
	inline jaw::fontid newFont(const font &o) { return newFont(&o); }

	// Returns jaw::INVALID_ID on failure
	jaw::bmpid createBmp(jaw::vec2i size);

	//Copies pixels into a bitmap. numPixels MUST equal the width x height of the bitmap
	//This routine is slow compared to others in the API, so better to call as little as needed
	bool writeBmp(jaw::bmpid, const jaw::argb *pixels, size_t numPixels);
	inline bool writeBmp(jaw::bmpid b, const jaw::argb *pixels, jaw::vec2i dim) {
		return writeBmp(b, pixels, dim.product());
	}

	// Creates a BMP in the same way as createBmp, but that is usable with draw::tobmp
	jaw::bmpid createRenderableBmp(jaw::vec2i size);

	// Immediately render to bitmap target for later use
	template <typename T>
	bool tobmp(const T&, jaw::bmpid);
	template<>
	bool tobmp<line>(const line&, jaw::bmpid bmp);
	template<>
	bool tobmp<rect>(const rect&, jaw::bmpid bmp);
	template<>
	bool tobmp<str>(const str&, jaw::bmpid);
	template<>
	bool tobmp<bmp>(const bmp&, jaw::bmpid);
	template<>
	bool tobmp<ellipse>(const ellipse&, jaw::bmpid);
}