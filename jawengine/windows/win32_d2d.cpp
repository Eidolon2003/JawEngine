#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d2d1.h>
#include <dwrite.h>
#include <cstdint>
#include <clocale>	//locale for wchar conversion
#include <cmath>	//ceilf
#include <cassert>	//assert
#include <malloc.h> //alloca
#include "../draw.h"
#include "win32_internal_draw.h"

#include "../common/avx2.h"	// multiplyAlpha_avx2

static const jaw::properties *props;
static D2D1_COLOR_F backgroundColor = D2D1::ColorF(0);

static ID2D1Factory *pD2DFactory = nullptr;
static ID2D1HwndRenderTarget *pRenderTarget = nullptr;
static ID2D1BitmapRenderTarget *pBitmapTarget = nullptr;
static ID2D1Bitmap *pBitmapTargetBMP = nullptr;
static IDWriteFactory *pDWFactory = nullptr;
static IDWriteRenderingParams *pParams = nullptr;
static ID2D1SolidColorBrush *pSolidBrush = nullptr;
static D2D1_BITMAP_INTERPOLATION_MODE interpMode;
static D2D1_ANTIALIAS_MODE AAMode;

static draw::drawCall writeQueue[draw::MAX_QUEUE_SIZE]{};
static draw::drawCall renderQueue[draw::MAX_QUEUE_SIZE]{};
static size_t writeQueueFront = 0;
static size_t renderQueueFront = 0;

static IDWriteTextFormat *fonts[draw::MAX_NUM_FONTS];
static size_t numFonts = 0;

static ID2D1Bitmap *bmps[draw::MAX_NUM_BMPS];
static ID2D1BitmapRenderTarget *bmpTargets[draw::MAX_NUM_BMPS];
static size_t numBmps = 0;

static wchar_t wstrBuffer[1024];
static size_t towstrbuf(const char *str) {
	if (str == nullptr) return static_cast<size_t>(-1);
	return mbstowcs(wstrBuffer, str, 1024);
}

static inline D2D1_POINT_2F topoint2f(const jaw::vec2i &v) {
	return D2D1::Point2F((float)v.x, (float)v.y);
}

static inline D2D1_RECT_F torectf(const jaw::recti &r) {
	return D2D1::RectF((float)r.tl.x, (float)r.tl.y, (float)r.br.x, (float)r.br.y);
}

static inline D2D1_COLOR_F tocolorf(const jaw::argb c) {
	return D2D1::ColorF(c, (c >> 24) / 255.f);
}

static inline float radtodeg(float a) {
	float deg = 360.f - (fmodf(a, 2*PI32) * (180.f / PI32));
	return fmodf(deg + 360.f, 360.f);
}

static void multiplyAlpha_fallback(jaw::argb *dst, const jaw::argb *src, size_t n) {
	const uint8_t *src_bytes = (const uint8_t*)src;
	uint8_t *dst_bytes = (uint8_t*)dst;
	for (size_t i = 0; i < n * 4; i += 4) {
		// ideally we'd round(x / 255)
		// (x + 128) >> 8 is pretty close and a lot faster
		dst_bytes[i+0] = (src_bytes[i+0] * src_bytes[i+3] + 128) >> 8;
		dst_bytes[i+1] = (src_bytes[i+1] * src_bytes[i+3] + 128) >> 8;
		dst_bytes[i+2] = (src_bytes[i+2] * src_bytes[i+3] + 128) >> 8;
		dst_bytes[i+3] = src_bytes[i+3];
	}
}
static auto multiplyAlpha = multiplyAlpha_fallback;

void draw::init(const jaw::properties *p, HWND hwnd) {
	setlocale(LC_ALL, "en_US.UTF-8");	//Needed for wchar_t conversion
	props = p;

	if (props->cpuid.avx2) {
		multiplyAlpha = multiplyAlpha_avx2;
	}

	// Direct2D Setup
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
		),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(props->winsize.x, props->winsize.y),
			props->targetFramerate <= 0 ?
			D2D1_PRESENT_OPTIONS_NONE : D2D1_PRESENT_OPTIONS_IMMEDIATELY
		),
		&pRenderTarget
	);

	pRenderTarget->CreateCompatibleRenderTarget(
		D2D1::SizeF((float)props->size.x, (float)props->size.y),
		&pBitmapTarget
	);

	AAMode = props->enablePerPrimitiveAA ?
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED;

	pBitmapTarget->SetAntialiasMode(AAMode);
	pBitmapTarget->GetBitmap(&pBitmapTargetBMP);

	pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&pSolidBrush
	);

	// DirectWrite Setup
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(pDWFactory),
		reinterpret_cast<IUnknown**>(&pDWFactory)
	);

	HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	pDWFactory->CreateMonitorRenderingParams(mon, &pParams);
	pDWFactory->CreateCustomRenderingParams(
		pParams->GetGamma(),
		pParams->GetEnhancedContrast(),
		pParams->GetClearTypeLevel(),
		pParams->GetPixelGeometry(),
		props->enableSubpixelTextRendering ?
		DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC : DWRITE_RENDERING_MODE_ALIASED,
		&pParams
	);
	pBitmapTarget->SetTextRenderingParams(pParams);

	// Get interpolation mode based on selected window mode and scale
	switch (props->mode) {
	case jaw::properties::WINDOWED:
	case jaw::properties::FULLSCREEN_CENTERED:
	case jaw::properties::FULLSCREEN_CENTERED_INTEGER: {
		if (props->scale == ceilf(props->scale))
			interpMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		else
			interpMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	}	break;

	case jaw::properties::FULLSCREEN_STRETCHED: {
		float scaleX = (float)props->winsize.x / props->size.x;
		float scaleY = (float)props->winsize.y / props->size.y;
		if (scaleX == ceilf(scaleX) && scaleY == ceilf(scaleY))
			interpMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		else
			interpMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	}	break;
	}

	// Create the deafault font as font zero
	auto f = font();
	auto id = draw::newFont(&f);
	assert(id == 0);
}

void draw::deinit() {
	pBitmapTargetBMP->Release();
	pBitmapTargetBMP = nullptr;
	pBitmapTarget->Release();
	pBitmapTarget = nullptr;
	pSolidBrush->Release();
	pSolidBrush = nullptr;
	pParams->Release();
	pParams = nullptr;
	pDWFactory->Release();
	pDWFactory = nullptr;
	pRenderTarget->Release();
	pRenderTarget = nullptr;
	pD2DFactory->Release();
	pD2DFactory = nullptr;

	for (size_t i = 0; i < numFonts; i++) {
		fonts[i]->Release();
	}
	numFonts = 0;

	for (size_t i = 0; i < numBmps; i++) {
		bmps[i]->Release();
	}
	numBmps = 0;

	props = nullptr;
	backgroundColor = tocolorf(0);
	writeQueueFront = renderQueueFront = 0;
}

//Sort from the writeQueue into the renderQueue, reset writeQueue
void draw::prepareRender() {
	//Counting sort:
	size_t counts[256] = { 0 };

	//Step 1: Count number of calls for each possible z
	for (size_t i = 0; i < writeQueueFront; i++) {
		counts[writeQueue[i].z]++;
	}

	//Step 2: Calculate indicies based on these counts
	size_t sum = 0;
	size_t tmp = 0;
	for (size_t &count : counts) {
		tmp = count;
		count = sum;
		sum += tmp;
	}

	//Step 3: Place draw calls into renderQueue in sorted order
	for (size_t i = 0; i < writeQueueFront; i++) {
		renderQueue[counts[writeQueue[i].z]++] = writeQueue[i];
	}

	//Finalize sizes
	renderQueueFront = writeQueueFront;
	writeQueueFront = 0;
}

static void inline renderLine(const draw::drawCall &c, ID2D1BitmapRenderTarget *target) {
	pSolidBrush->SetColor(tocolorf(c.line.color));

	auto mid = (c.line.p1 + c.line.p2) / 2;
	target->SetTransform(D2D1::Matrix3x2F::Rotation(radtodeg(c.line.angle), topoint2f(mid)));

	target->DrawLine(
		topoint2f(c.line.p1),
		topoint2f(c.line.p2),
		pSolidBrush,
		(float)c.line.width
	);
}

static void inline renderRect(const draw::drawCall &c, ID2D1BitmapRenderTarget *target) {
	pSolidBrush->SetColor(tocolorf(c.rect.color));

	auto mid = (c.rect.rect.tl + c.rect.rect.br) / 2;
	target->SetTransform(D2D1::Matrix3x2F::Rotation(radtodeg(c.rect.angle), topoint2f(mid)));

	target->FillRectangle(
		torectf(c.rect.rect),
		pSolidBrush
	);
}

static void inline renderStr(const draw::drawCall &c, ID2D1BitmapRenderTarget *target) {
	pSolidBrush->SetColor(tocolorf(c.str.color));

	auto mid = (c.str.rect.tl + c.str.rect.br) / 2;
	target->SetTransform(D2D1::Matrix3x2F::Rotation(radtodeg(c.str.angle), topoint2f(mid)));

	auto len = towstrbuf(c.str.str);
	target->DrawText(
		wstrBuffer,
		(UINT32)len,
		fonts[c.str.font],
		torectf(c.str.rect),
		pSolidBrush
	);
}

//TODO: Needs options for alpha and interp mode
static void inline renderBmp(const draw::drawCall &c, ID2D1BitmapRenderTarget *target) {
	if (c.bmp.bmp >= numBmps) return;

	auto mid = (c.bmp.dest.tl + c.bmp.dest.br) / 2;
	auto transform = D2D1::Matrix3x2F::Identity();
	auto transDest = c.bmp.dest;

	if (c.bmp.mirrorX) {
		transform = transform * D2D1::Matrix3x2F::Scale(-1.f, 1.f);
		transDest.br.x = -c.bmp.dest.br.x;
		transDest.tl.x = -c.bmp.dest.tl.x;
	}

	if (c.bmp.mirrorY) {
		transform = transform * D2D1::Matrix3x2F::Scale(1.f, -1.f);
		transDest.br.y = -c.bmp.dest.br.y;
		transDest.tl.y = -c.bmp.dest.tl.y;
	}

	transform = transform * D2D1::Matrix3x2F::Rotation(radtodeg(c.bmp.angle), topoint2f(mid));
	target->SetTransform(transform);

	target->DrawBitmap(
		bmps[c.bmp.bmp],
		torectf(transDest),
		1.f,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		torectf(c.bmp.src)
	);
}

static void inline renderEllipse(const draw::drawCall &c, ID2D1BitmapRenderTarget *target) {
	pSolidBrush->SetColor(tocolorf(c.ellipse.color));

	target->SetTransform(D2D1::Matrix3x2F::Rotation(radtodeg(c.ellipse.angle), topoint2f(c.ellipse.ellipse.center)));
	
	target->FillEllipse(
		D2D1::Ellipse(
			topoint2f(c.ellipse.ellipse.center),
			(float)c.ellipse.ellipse.radii.x,
			(float)c.ellipse.ellipse.radii.y
		),
		pSolidBrush
	);
}

static void inline renderAny(const draw::drawCall &c, ID2D1BitmapRenderTarget *target) {
	switch (c.t) {
	case draw::type::LINE:
		renderLine(c, target);
		break;

	case draw::type::RECT:
		renderRect(c, target);
		break;

	case draw::type::STR:
		renderStr(c, target);
		break;

	case draw::type::BMP:
		renderBmp(c, target);
		break;

	case draw::type::ELLIPSE:
		renderEllipse(c, target);
		break;

	case draw::type::NUM_TYPES:
		break;
	}
}

void draw::render() {
	pBitmapTarget->BeginDraw();
	pBitmapTarget->Clear(backgroundColor);
	for (size_t i = 0; i < renderQueueFront; i++) {
		renderAny(renderQueue[i], pBitmapTarget);
	}
	pBitmapTarget->EndDraw();

	pRenderTarget->BeginDraw();
	switch (props->mode) {
	case jaw::properties::WINDOWED:
	case jaw::properties::FULLSCREEN_STRETCHED: {
		pRenderTarget->DrawBitmap(
			pBitmapTargetBMP,
			D2D1::RectF(
				0.f,
				0.f,
				(float)props->winsize.x,
				(float)props->winsize.y
			),
			1.f,
			interpMode,
			D2D1::RectF(
				0.f,
				0.f,
				(float)props->size.x,
				(float)props->size.y
			)
		);
	}	break;

	case jaw::properties::FULLSCREEN_CENTERED:
	case jaw::properties::FULLSCREEN_CENTERED_INTEGER: {
		int16_t offsetX = (props->winsize.x - props->scaledSize().x) / 2;
		int16_t offsetY = (props->winsize.y - props->scaledSize().y) / 2;
		pRenderTarget->DrawBitmap(
			pBitmapTargetBMP,
			D2D1::RectF(
				(float)offsetX,
				(float)offsetY,
				(float)props->scaledSize().x + offsetX,
				(float)props->scaledSize().y + offsetY
			),
			1.f,
			interpMode,
			D2D1::RectF(
				0.f,
				0.f,
				(float)props->size.x,
				(float)props->size.y
			)
		);
	}	break;
	}
}

void draw::present() {
	// Blocks until VBLANK if Vsync is enabled
	pRenderTarget->EndDraw();
}

void draw::setBackgroundColor(jaw::argb color) {
	backgroundColor = tocolorf(color);
}

jaw::fontid draw::newFont(const draw::font *opt) {
	if (numFonts == draw::MAX_NUM_FONTS) return jaw::INVALID_ID;
	assert(opt != nullptr);

	auto i = numFonts++;
	auto _ = towstrbuf(opt->name);
	pDWFactory->CreateTextFormat(
		wstrBuffer,
		NULL,
		opt->bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
		opt->italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		opt->size,
		L"en-us",
		fonts + i
	);

	switch (opt->align) {
	case draw::font::LEFT:
		fonts[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;

	case draw::font::RIGHT:
		fonts[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;

	case draw::font::CENTER:
		fonts[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	}
	return (jaw::fontid)i;
}

jaw::bmpid draw::createBmp(jaw::vec2i size) {
	if (numBmps == draw::MAX_NUM_BMPS) return jaw::INVALID_ID;

	float dpix, dpiy;
	pRenderTarget->GetDpi(&dpix, &dpiy);

	HRESULT hr = pRenderTarget->CreateBitmap(
		D2D1_SIZE_U(size.x, size.y),
		D2D1::BitmapProperties(
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
			),
			dpix,
			dpiy
		),
		bmps + numBmps
	);
	if (!SUCCEEDED(hr)) {
		return jaw::INVALID_ID;
	}

	return (jaw::bmpid)numBmps++;
}

jaw::bmpid draw::createRenderableBmp(jaw::vec2i size) {
	if (numBmps == draw::MAX_NUM_BMPS) return jaw::INVALID_ID;

	HRESULT hr = pBitmapTarget->CreateCompatibleRenderTarget(
		D2D1::SizeF((float)size.x, (float)size.y),
		bmpTargets + numBmps
	);
	if (!SUCCEEDED(hr)) {
		return jaw::INVALID_ID;
	}

	bmpTargets[numBmps]->SetAntialiasMode(AAMode);
	bmpTargets[numBmps]->SetTextRenderingParams(pParams);

	hr = bmpTargets[numBmps]->GetBitmap(bmps + numBmps);
	if (!SUCCEEDED(hr)) {
		bmpTargets[numBmps]->Release();
		return jaw::INVALID_ID;
	}

	return (jaw::bmpid)numBmps++;
}

#if 1
bool draw::writeBmp(jaw::bmpid bmp, const jaw::argb *pixels, size_t numPixels) {
	if (bmp >= numBmps) return false;
	assert(pixels != nullptr);

	// Allocate space for this array on the stack
	jaw::argb *multiplied = (jaw::argb*)_malloca(numPixels * sizeof(jaw::argb));
	if (!multiplied) { return false; }

	multiplyAlpha(multiplied, pixels, numPixels);

	auto size = bmps[bmp]->GetPixelSize();
	D2D1_RECT_U destRect {};
	destRect.left = destRect.top = 0;
	destRect.right = size.width;
	destRect.bottom = size.height;

	assert((destRect.right - destRect.left) * (destRect.bottom - destRect.top) == numPixels);

	HRESULT hr = bmps[bmp]->CopyFromMemory(
		&destRect,
		multiplied,
		size.width * sizeof(jaw::argb)
	);
	_freea(multiplied);
	return SUCCEEDED(hr);
}
#else
// This version is for testing if the avx multiply routine produces the right answer
bool draw::writeBmp(jaw::bmpid bmp, const jaw::argb *pixels, size_t numPixels) {
	if (bmp >= numBmps) return false;
	assert(pixels != nullptr);

	// Allocate space for this array on the stack
	jaw::argb *multiplied1 = (jaw::argb*)_malloca(numPixels * sizeof(jaw::argb));
	if (!multiplied1) { return false; }

	jaw::argb *multiplied2 = (jaw::argb*)_malloca(numPixels * sizeof(jaw::argb));
	if (!multiplied2) { return false; }

	multiplyAlpha_fallback(multiplied1, pixels, numPixels);
	multiplyAlpha_avx2(multiplied2, pixels, numPixels);
	assert(memcmp(multiplied1, multiplied2, numPixels * 4) == 0);

	auto size = bmps[bmp]->GetPixelSize();
	D2D1_RECT_U destRect {};
	destRect.left = destRect.top = 0;
	destRect.right = size.width;
	destRect.bottom = size.height;

	assert((destRect.right - destRect.left) * (destRect.bottom - destRect.top) == numPixels);

	HRESULT hr = bmps[bmp]->CopyFromMemory(
		&destRect,
		multiplied1,
		size.width * sizeof(jaw::argb)
	);
	_freea(multiplied1);
	_freea(multiplied2);
	return SUCCEEDED(hr);
}
#endif

template<>
draw::drawCall draw::make<draw::line>(const draw::line &x, uint8_t z) {
	draw::drawCall c{};
	c.t = draw::type::LINE;
	c.z = z;
	c.line = x;
	return c;
}

template<>
draw::drawCall draw::make<draw::rect>(const draw::rect &x, uint8_t z) {
	draw::drawCall c{};
	c.t = draw::type::RECT;
	c.z = z;
	c.rect = x;
	return c;
}

template<>
draw::drawCall draw::make<draw::str>(const draw::str &x, uint8_t z) {
	draw::drawCall c{};
	c.t = draw::type::STR;
	c.z = z;
	c.str = x;
	return c;
}

template<>
draw::drawCall draw::make<draw::bmp>(const draw::bmp &x, uint8_t z) {
	draw::drawCall c{};
	c.t = draw::type::BMP;
	c.z = z;
	c.bmp = x;
	return c;
}

template<>
draw::drawCall draw::make<draw::ellipse>(const draw::ellipse &x, uint8_t z) {
	draw::drawCall c{};
	c.t = draw::type::ELLIPSE;
	c.z = z;
	c.ellipse = x;
	return c;
}

template<>
bool draw::enqueue<draw::line>(const draw::line &x, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = make(x, z);
	return true;
}

template<>
bool draw::enqueue<draw::rect>(const draw::rect &x, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = make(x, z);
	return true;
}

template<>
bool draw::enqueue<draw::str>(const draw::str &x, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = make(x, z);
	return true;
}

template<>
bool draw::enqueue<draw::bmp>(const draw::bmp &x, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = make(x, z);
	return true;
}

template<>
bool draw::enqueue<draw::ellipse>(const draw::ellipse &x, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = make(x, z);
	return true;
}

bool draw::enqueueMany(const draw::drawCall *c, size_t l) {
	assert(c != nullptr);
	if (writeQueueFront + l > MAX_QUEUE_SIZE) return false;
	memcpy(writeQueue + writeQueueFront, c, sizeof(draw::drawCall) * l);
	writeQueueFront += l;
	return true;
}

template <>
bool draw::tobmp<draw::line>(const draw::line &x, jaw::bmpid bmp) {
	if (!bmpTargets[bmp]) return false;
	auto &target = bmpTargets[bmp];
	target->BeginDraw();
	renderAny(make(x, 0), target);
	target->EndDraw();
	return true;
}

template <>
bool draw::tobmp<draw::rect>(const draw::rect &x, jaw::bmpid bmp) {
	if (!bmpTargets[bmp]) return false;
	auto &target = bmpTargets[bmp];
	target->BeginDraw();
	renderAny(make(x, 0), target);
	target->EndDraw();
	return true;
}

template <>
bool draw::tobmp<draw::str>(const draw::str &x, jaw::bmpid bmp) {
	if (!bmpTargets[bmp]) return false;
	auto &target = bmpTargets[bmp];
	target->BeginDraw();
	renderAny(make(x, 0), target);
	target->EndDraw();
	return true;
}

template <>
bool draw::tobmp<draw::bmp>(const draw::bmp &x, jaw::bmpid bmp) {
	if (!bmpTargets[bmp]) return false;
	auto &target = bmpTargets[bmp];
	target->BeginDraw();
	renderAny(make(x, 0), target);
	target->EndDraw();
	return true;
}

template <>
bool draw::tobmp<draw::ellipse>(const draw::ellipse &x, jaw::bmpid bmp) {
	if (!bmpTargets[bmp]) return false;
	auto &target = bmpTargets[bmp];
	target->BeginDraw();
	renderAny(make(x, 0), target);
	target->EndDraw();
	return true;
}