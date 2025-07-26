#define _CRT_SECURE_NO_WARNINGS
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <cstdint>
#include <clocale>	//locale for wchar conversion
#include <cmath>	//ceilf
#include "draw.h"
#include "win32_internal_draw.h"

static const jaw::properties* props;
static D2D1_COLOR_F backgroundColor = D2D1::ColorF(0);

static ID2D1Factory* pD2DFactory = nullptr;
static ID2D1HwndRenderTarget* pRenderTarget = nullptr;
static ID2D1BitmapRenderTarget* pBitmapTarget = nullptr;
static ID2D1Bitmap* pBitmapTargetBMP = nullptr;
static IDWriteFactory* pDWFactory = nullptr;
static IDWriteRenderingParams* pParams = nullptr;
static ID2D1SolidColorBrush* pSolidBrush = nullptr;
static IWICImagingFactory* pIWICFactory = nullptr;
static D2D1_BITMAP_INTERPOLATION_MODE interpMode;

static draw::drawCall writeQueue[draw::MAX_QUEUE_SIZE];
static draw::drawCall renderQueue[draw::MAX_QUEUE_SIZE];
static size_t writeQueueFront = 0;
static size_t renderQueueFront = 0;

static IDWriteTextFormat* fonts[draw::MAX_NUM_FONTS];
static size_t numFonts = 0;

static ID2D1Bitmap* bmps[draw::MAX_NUM_BMPS];
static size_t numBmps = 0;

static wchar_t wstrBuffer[1024];
size_t towstrbuf(const char* str) {
	if (str == nullptr) return static_cast<size_t>(-1);
	return mbstowcs(wstrBuffer, str, 1024);
}

inline D2D1_POINT_2F topoint2f(const jaw::vec2i& v) {
	return D2D1::Point2F((float)v.x, (float)v.y);
}

inline D2D1_RECT_F torectf(const jaw::recti& r) {
	return D2D1::RectF((float)r.tl.x, (float)r.tl.y, (float)r.br.x, (float)r.br.y);
}

inline D2D1_COLOR_F tocolorf(const draw::argb c) {
	return D2D1::ColorF(c, (c >> 24) / 255.f);
}

void draw::init(const jaw::properties* p, HWND hwnd) {
	setlocale(LC_ALL, "en_US.UTF-8");	//Needed for wchar_t conversion
	props = p;

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
			D2D1_PRESENT_OPTIONS_IMMEDIATELY
		),
		&pRenderTarget
	);

	pRenderTarget->CreateCompatibleRenderTarget(
		D2D1::SizeF((float)props->size.x, (float)props->size.y),
		&pBitmapTarget
	);
	pBitmapTarget->SetAntialiasMode(
		props->enablePerPrimitiveAA ? 
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED
	);
	pBitmapTarget->GetBitmap(&pBitmapTargetBMP);

	pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&pSolidBrush
	);

	CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&pIWICFactory
	);

	// DirectWrite Setup
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(pDWFactory),
		reinterpret_cast<IUnknown**>(&pDWFactory)
	);

	HMONITOR primaryMonitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
	pDWFactory->CreateMonitorRenderingParams(primaryMonitor, &pParams);
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
	auto f = fontOptions();
	auto _ = draw::newFont(&f);
}

void draw::deinit() {
	pBitmapTargetBMP->Release();
	pBitmapTarget->Release();
	pIWICFactory->Release();
	pSolidBrush->Release();
	pParams->Release();
	pDWFactory->Release();
	pRenderTarget->Release();
	pD2DFactory->Release();

	for (int i = 0; i < numFonts; i++) {
		fonts[i]->Release();
	}

	for (int i = 0; i < numBmps; i++) {
		bmps[i]->Release();
	}
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
	for (size_t& count : counts) {
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

static void inline renderLine(draw::drawCall& c) {
	draw::lineOptions* opt = (draw::lineOptions*)(c.data);
	pSolidBrush->SetColor(tocolorf(opt->color));

	float offset = opt->width % 2 ? 0.5f : 0.f;
	opt->p1 = opt->p1 + (int16_t)offset;
	opt->p2 = opt->p2 + (int16_t)offset;

	pBitmapTarget->DrawLine(
		topoint2f(opt->p1),
		topoint2f(opt->p2),
		pSolidBrush,
		(float)opt->width
	);
}

static void inline renderRect(draw::drawCall& c) {
	draw::rectOptions* opt = (draw::rectOptions*)(c.data);
	pSolidBrush->SetColor(tocolorf(opt->color));
	pBitmapTarget->FillRectangle(
		torectf(opt->rect),
		pSolidBrush
	);
}

static void inline renderStr(draw::drawCall& c) {
	draw::strOptions* opt = (draw::strOptions*)(c.data);
	pSolidBrush->SetColor(tocolorf(opt->color));
	auto len = towstrbuf(opt->str);
	pBitmapTarget->DrawText(
		wstrBuffer,
		(UINT32)len,
		fonts[opt->font],
		torectf(opt->rect),
		pSolidBrush
	);
}

//TODO: Needs options for alpha and interp mode
static void inline renderBmp(draw::drawCall& c) {
	draw::bmpOptions* opt = (draw::bmpOptions*)(c.data);
	pBitmapTarget->DrawBitmap(
		bmps[opt->bmp],
		torectf(opt->dest),
		1.f,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		torectf(opt->src)
	);
}

static void inline renderEllipse(draw::drawCall& c) {
	draw::ellipseOptions* opt = (draw::ellipseOptions*)(c.data);
	pSolidBrush->SetColor(tocolorf(opt->color));
	pBitmapTarget->FillEllipse(
		D2D1::Ellipse(
			topoint2f(opt->ellipse.center),
			(float)opt->ellipse.radii.x,
			(float)opt->ellipse.radii.y
		),
		pSolidBrush
	);
}

void draw::render() {
	pBitmapTarget->BeginDraw();
	pBitmapTarget->Clear(backgroundColor);
	for (size_t i = 0; i < renderQueueFront; i++) {
		draw::drawCall& call = renderQueue[i];
		switch (call.t) {
		case draw::type::LINE:
			renderLine(call);
			break;
		
		case draw::type::RECT:
			renderRect(call);
			break;

		case draw::type::STR:
			renderStr(call);
			break;

		case draw::type::BMP:
			renderBmp(call);
			break;

		case draw::type::ELLIPSE:
			renderEllipse(call);
			break;
		}
	}
	pBitmapTarget->EndDraw();

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear();

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

	pRenderTarget->EndDraw();
}

void draw::setBackgroundColor(draw::argb color) {
	backgroundColor = tocolorf(color);
}

draw::fontid draw::newFont(const draw::fontOptions* opt) {
	if (numFonts == draw::MAX_NUM_FONTS) return (fontid)draw::MAX_NUM_FONTS;
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
	case draw::fontOptions::LEFT:
		fonts[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;

	case draw::fontOptions::RIGHT:
		fonts[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;

	case draw::fontOptions::CENTER:
		fonts[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	}
	return (fontid)i;
}

draw::bmpid draw::loadBmp(const char* filename) {
	if (numBmps == draw::MAX_NUM_BMPS) return (bmpid)draw::MAX_NUM_BMPS;

	auto _ = towstrbuf(filename);
	IWICBitmapDecoder* decoder;
	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		wstrBuffer,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);
	if (!SUCCEEDED(hr)) {
		return (bmpid)draw::MAX_NUM_BMPS;
	}

	IWICFormatConverter* converter;
	hr = pIWICFactory->CreateFormatConverter(&converter);
	if (!SUCCEEDED(hr)) {
		decoder->Release();
		return (bmpid)draw::MAX_NUM_BMPS;
	}

	IWICBitmapFrameDecode* frame;
	hr = decoder->GetFrame(0, &frame);
	if (!SUCCEEDED(hr)) {
		decoder->Release();
		converter->Release();
		return (bmpid)draw::MAX_NUM_BMPS;
	}

	hr = converter->Initialize(
		frame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeCustom
	);
	if (!SUCCEEDED(hr)) {
		decoder->Release();
		converter->Release();
		frame->Release();
		return (bmpid)draw::MAX_NUM_BMPS;
	}

	pRenderTarget->CreateBitmapFromWicBitmap(
		converter,
		NULL,
		bmps + numBmps
	);

	converter->Release();
	frame->Release();
	decoder->Release();
	return (bmpid)numBmps++;
}

draw::bmpid draw::createBmp(jaw::vec2i size) {
	if (numBmps == draw::MAX_NUM_BMPS) return (bmpid)draw::MAX_NUM_BMPS;

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
		return (bmpid)draw::MAX_NUM_BMPS;
	}

	return (bmpid)numBmps++;
}

//TODO: Support alpha transparency
bool draw::writeBmp(draw::bmpid bmp, const draw::argb* pixels) {
	if (bmp >= numBmps) return false;

	auto size = bmps[bmp]->GetPixelSize();
	D2D1_RECT_U destRect;
	destRect.left = destRect.top = 0;
	destRect.right = size.width;
	destRect.bottom = size.height;

	HRESULT hr = bmps[bmp]->CopyFromMemory(
		&destRect,
		pixels,
		size.width * sizeof(draw::argb)
	);
	return SUCCEEDED(hr);
}

//TODO: see if I can do better than memcpy with wide registers or optimizing for 32 bytes, maybe?

draw::drawCall draw::makeDraw(draw::type t, uint8_t z, const void* opt) {
	draw::drawCall x = {};
	x.t = t;
	x.z = z;
	memcpy(x.data, opt, draw::typeSizes[t]);
	return x;
}

bool draw::enqueue(const draw::drawCall* c) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	memcpy(writeQueue + writeQueueFront, c, sizeof(draw::drawCall));
	writeQueueFront++;
	return true;
}

bool draw::enqueueMany(const draw::drawCall* c, size_t l) {
	if (writeQueueFront + l > MAX_QUEUE_SIZE) return false;
	memcpy(writeQueue + writeQueueFront, c, sizeof(draw::drawCall) * l);
	writeQueueFront += l;
	return true;
}

bool draw::line(const draw::lineOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::LINE, z, opt);
	return true;
}

bool draw::rect(const draw::rectOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::RECT, z, opt);
	return true;
}

bool draw::str(const draw::strOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	if (opt->font >= numFonts) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::STR, z, opt);
	return true;
}

bool draw::bmp(const draw::bmpOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	if (opt->bmp >= numBmps) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::BMP, z, opt);
	return true;
}

bool draw::ellipse(const draw::ellipseOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::ELLIPSE, z, opt);
	return true;
}