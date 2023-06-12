#include "d2d.h"

/*
	DIRECT2D SETUP
*/

jaw::D2DGraphics::D2DGraphics(HWND hWnd, uint16_t x, uint16_t y) {
	setSize(x, y);
	this->hWnd = hWnd;
	backgroundColor = 0x000000;
	layers.clear();
	bitmaps.clear();

	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
		),
		D2D1::HwndRenderTargetProperties(
			hWnd,
			D2D1::SizeU(sizeX, sizeY),
			D2D1_PRESENT_OPTIONS_IMMEDIATELY
		),
		&pRenderTarget
	);

	for (int i = 0; i < LAYERS; i++) {
		layers.push_back(nullptr);
		pRenderTarget->CreateCompatibleRenderTarget(&layers[i]);
	}

	pSolidBrush = nullptr;
	pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&pSolidBrush
	);
}

jaw::D2DGraphics::~D2DGraphics() {
	for (auto& [n, p] : bitmaps) {
		//p->pBitmap->Release();
		delete p;
	}
	bitmaps.clear();

	pSolidBrush->Release();
	for (auto x : layers) {
		x->EndDraw();
		x->Release();
	}
	pRenderTarget->EndDraw();
	pRenderTarget->Release();
	pD2DFactory->Release();
}

void jaw::D2DGraphics::setSize(uint16_t x, uint16_t y) {
	sizeX = x;
	sizeY = y;
}

void jaw::D2DGraphics::BeginFrame() {
	layers[0]->BeginDraw();

	for (int i = 1; i < LAYERS; i++) {
		layers[i]->BeginDraw();
		layers[i]->Clear(D2D1::ColorF(0, 0, 0, 0));
	}
}

void jaw::D2DGraphics::EndFrame() {
	for (auto x : layers)
		x->EndDraw();

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(backgroundColor));

	for (auto x : layers) {
		ID2D1Bitmap* pBitmap = nullptr;
		x->GetBitmap(&pBitmap);
		if (!pBitmap) continue;
		pRenderTarget->DrawBitmap(
			pBitmap,
			D2D1::Rect(
				0.f,
				0.f,
				(float)sizeX,
				(float)sizeY
			),
			1.0,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
		);
		pBitmap->Release();
	}
	pRenderTarget->EndDraw();
}


/*
	BITMAPS
*/

jaw::D2DGraphics::D2DBitmap::D2DBitmap(std::string n, ID2D1Bitmap* p) {
	pBitmap = p;
	name = n;
	x = (uint32_t)(p->GetSize().width);
	y = (uint32_t)(p->GetSize().height);
}

std::string jaw::D2DGraphics::D2DBitmap::getName() {
	return name;
}

std::pair<uint32_t, uint32_t> jaw::D2DGraphics::D2DBitmap::getSize() {
	return std::pair<uint32_t, uint32_t>(x, y);
}

jaw::D2DGraphics::Bitmap* jaw::D2DGraphics::LoadBmp(std::string filename) {
	if (bitmaps.count(filename)) return bitmaps[filename];

	IWICImagingFactory* pIWICFactory = nullptr;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pFrame = nullptr;
	IWICFormatConverter* pFormatConverter = nullptr;
	ID2D1Bitmap* pBitmap = nullptr;

	D2DBitmap* p = nullptr;

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(void**)&pIWICFactory
	);
	if (!SUCCEEDED(hr)) return nullptr;

	hr = pIWICFactory->CreateDecoderFromFilename(
		std::wstring(filename.begin(), filename.end()).c_str(),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&pDecoder
	);
	if (!SUCCEEDED(hr)) goto LoadBmp_return1;

	hr = pDecoder->GetFrame(0, &pFrame);
	if (!SUCCEEDED(hr)) goto LoadBmp_return2;

	hr = pIWICFactory->CreateFormatConverter(&pFormatConverter);
	if (!SUCCEEDED(hr)) goto LoadBmp_return3;

	hr = pFormatConverter->Initialize(
		pFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeCustom
	);
	if (!SUCCEEDED(hr)) goto LoadBmp_return4;

	hr = pRenderTarget->CreateBitmapFromWicBitmap(
		pFormatConverter,
		NULL,
		&pBitmap
	);
	if (!SUCCEEDED(hr)) goto LoadBmp_return4;

	p = new D2DBitmap(filename, pBitmap);
	bitmaps[filename] = p;

LoadBmp_return4:
	pFormatConverter->Release();
LoadBmp_return3:
	pFrame->Release();
LoadBmp_return2:
	pDecoder->Release();
LoadBmp_return1:
	pIWICFactory->Release();
	return p;
}

void jaw::D2DGraphics::DrawBmp(std::string filename, uint16_t x, uint16_t y, uint8_t layer, float scale, float opacity, bool interpolation) {
	if (!bitmaps.count(filename))
		if (!LoadBmp(filename)) return;

	D2DBitmap* pBitmap = bitmaps[filename];

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->DrawBitmap(
		pBitmap->pBitmap,
		D2D1::Rect(
			(float)x,
			(float)y,
			x + (pBitmap->x * scale),
			y + (pBitmap->y * scale)
		),
		opacity,
		interpolation ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
	);
}


/*
	GRAPHICS ROUTINES
*/

void jaw::D2DGraphics::setBackgroundColor(uint32_t color) {
	backgroundColor = color;
}

void jaw::D2DGraphics::ClearLayer(uint8_t layer, uint32_t color, float alpha) {
	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->Clear(D2D1::ColorF(color, alpha));
}

void jaw::D2DGraphics::FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, uint8_t layer) {
	pSolidBrush->SetColor(D2D1::ColorF(color));

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->FillRectangle(
		D2D1::Rect(
			(float)x1,
			(float)y1,
			(float)x2,
			(float)y2
		),
		pSolidBrush
	);
}