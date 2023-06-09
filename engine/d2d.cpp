#include "d2d.h"

jaw::D2DGraphics::D2DGraphics(HWND hWnd, uint16_t x, uint16_t y) {

	setSize(x, y);
	this->hWnd = hWnd;

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

	for (int i = 0; i < 8; i++) {
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
	for (auto x : layers) {
		x->BeginDraw();
		x->Clear(D2D1::ColorF(0, 0, 0, 0));
	}
}

void jaw::D2DGraphics::EndFrame() {
	for (auto x : layers)
		x->EndDraw();

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

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

void jaw::D2DGraphics::FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, uint8_t layer) {
	pSolidBrush->SetColor(D2D1::ColorF(color));

	if (layer > 7) layer = 7;
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