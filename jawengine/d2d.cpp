#define _CRT_SECURE_NO_WARNINGS
#include <d2d1.h>
#include <dwrite.h>
#include "draw.h"
#include "windraw_internal.h"

static jaw::vec2i renderSize = { 0 };
static jaw::vec2i windowSize = { 0 };
float scale = 0.f;

static ID2D1Factory* pD2DFactory = nullptr;
static ID2D1HwndRenderTarget* pRenderTarget = nullptr;
static ID2D1BitmapRenderTarget* pBitmapTarget = nullptr;
static IDWriteFactory* pDWFactory = nullptr;
static IDWriteRenderingParams* pParams = nullptr;
static ID2D1SolidColorBrush* pSolidBrush = nullptr;

static draw::drawCall writeQueue[draw::MAX_QUEUE_SIZE];
static draw::drawCall renderQueue[draw::MAX_QUEUE_SIZE];
static size_t writeQueueFront = 0;
static size_t renderQueueFront = 0;

static IDWriteTextFormat* fonts[draw::MAX_NUM_FONTS];
static size_t numFonts = 0;

static wchar_t wstrBuffer[1024];
size_t towstr(const char* str) {
	if (str == nullptr) return static_cast<std::size_t>(-1);
	return mbstowcs(wstrBuffer, str, 1024);
}

void draw::init(jaw::properties* props, HWND hwnd) {
	setlocale(LC_ALL, "en_US.UTF-8");	//Needed for wchar_t conversion
	renderSize = props->size;
	windowSize = props->winsize;
	scale = props->scale;

	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
		),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(windowSize.x, windowSize.y),
			D2D1_PRESENT_OPTIONS_IMMEDIATELY
		),
		&pRenderTarget
	);

	pRenderTarget->CreateCompatibleRenderTarget(
		D2D1::SizeF((float)renderSize.x, (float)renderSize.y),
		&pBitmapTarget
	);

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
		DWRITE_RENDERING_MODE_NATURAL : DWRITE_RENDERING_MODE_ALIASED,
		&pParams
	);
	pBitmapTarget->SetTextRenderingParams(pParams);

	pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&pSolidBrush
	);

	auto f = fontOptions();
	auto _ = draw::newFont(&f);
}

void draw::deinit() {
	pSolidBrush->Release();
	pParams->Release();
	pDWFactory->Release();
	pRenderTarget->Release();
	pD2DFactory->Release();

	for (int i = 0; i < numFonts; i++) {
		fonts[i]->Release();
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
	pSolidBrush->SetColor(D2D1::ColorF(opt->color, (opt->color >> 24) / 255.f));

	float offset = opt->width % 2 ? 0.5f : 0.f;

	pBitmapTarget->DrawLine(
		D2D1::Point2((float)(opt->p1.x) + offset, (float)(opt->p1.y) + offset),
		D2D1::Point2((float)(opt->p2.x) + offset, (float)(opt->p2.y) + offset),
		pSolidBrush,
		(float)opt->width
	);
}

static void inline renderRect(draw::drawCall& c) {
	draw::rectOptions* opt = (draw::rectOptions*)(c.data);
	pSolidBrush->SetColor(D2D1::ColorF(opt->color, (opt->color >> 24) / 255.f));

	pBitmapTarget->FillRectangle(
		D2D1::RectF(
			(float)opt->rect.tl.x,
			(float)opt->rect.tl.y,
			(float)opt->rect.br.x,
			(float)opt->rect.br.y
		),
		pSolidBrush
	);
}

static void inline renderStr(draw::drawCall& c) {
	draw::strOptions* opt = (draw::strOptions*)(c.data);
	if (opt->font >= numFonts) return;
	pSolidBrush->SetColor(D2D1::ColorF(opt->color, (opt->color >> 24) / 255.f));
	auto _ = towstr(opt->str);
	pBitmapTarget->DrawText(
		wstrBuffer,
		(UINT32)std::strlen(opt->str),
		fonts[opt->font],
		D2D1::RectF(
			(float)opt->rect.tl.x,
			(float)opt->rect.tl.y,
			(float)opt->rect.br.x,
			(float)opt->rect.br.y
		),
		pSolidBrush
	);
}

void draw::render() {
	pBitmapTarget->BeginDraw();
	pBitmapTarget->Clear();
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
		}
	}
	pBitmapTarget->EndDraw();

	auto mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	if (ceilf(scale) == scale) mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear();
	ID2D1Bitmap* bmp = nullptr;
	pBitmapTarget->GetBitmap(&bmp);
	pRenderTarget->DrawBitmap(
		bmp,
		D2D1::RectF(
			0.f,
			0.f,
			(float)windowSize.x,
			(float)windowSize.y
		),
		1.f,
		mode,
		D2D1::RectF(
			0.f,
			0.f,
			(float)renderSize.x,
			(float)renderSize.y
		)
	);
	bmp->Release();
	pRenderTarget->EndDraw();
}

draw::fontid draw::newFont(draw::fontOptions* opt) {
	if (numFonts == draw::MAX_NUM_FONTS) return (fontid)draw::MAX_NUM_FONTS;
	auto i = numFonts++;
	auto _ = towstr(opt->name);
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

static size_t typeSizes[] = { 
	sizeof(draw::lineOptions),
	sizeof(draw::rectOptions),
	sizeof(draw::strOptions)
};
draw::drawCall draw::makeDraw(draw::type t, uint8_t z, void* opt) {
	draw::drawCall x = {};
	x.t = t;
	x.z = z;
	std::memcpy(x.data, opt, typeSizes[t]);
	return x;
}

bool draw::enqueue(draw::drawCall* c) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	std::memcpy(writeQueue + writeQueueFront, c, sizeof(draw::drawCall));
	writeQueueFront++;
	return true;
}

bool draw::enqueueMany(draw::drawCall* c, size_t l) {
	if (writeQueueFront + l > MAX_QUEUE_SIZE) return false;
	std::memcpy(writeQueue + writeQueueFront, c, sizeof(draw::drawCall) * l);
	writeQueueFront += l;
	return true;
}

bool draw::line(draw::lineOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::LINE, z, opt);
	return true;
}

bool draw::rect(draw::rectOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::RECT, z, opt);
	return true;
}

bool draw::str(draw::strOptions* opt, uint8_t z) {
	if (writeQueueFront == MAX_QUEUE_SIZE) return false;
	writeQueue[writeQueueFront++] = makeDraw(draw::type::STR, z, opt);
	return true;
}