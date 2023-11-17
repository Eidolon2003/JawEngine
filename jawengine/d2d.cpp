#include "d2d.h"

/*
	DIRECT2D SETUP
*/

jaw::D2DGraphics::D2DGraphics(HWND hWnd, uint16_t x, uint16_t y, float scale, std::wstring locale) {
	setSize(x, y);
	this->scale = scale;
	this->hWnd = hWnd;
	this->locale = locale;
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
			D2D1::SizeU(ScaleUp(sizeX, scale), ScaleUp(sizeY, scale)),
			D2D1_PRESENT_OPTIONS_IMMEDIATELY
		),
		&pRenderTarget
	);

	for (int i = 0; i < LAYERS; i++) {
		layers.push_back(nullptr);
		layersChanged.push_back(false);
		pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(sizeX, sizeY), &layers[i]);
		layers[i]->BeginDraw();
	}

	pSolidBrush = nullptr;
	pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&pSolidBrush
	);

	pDWFactory = nullptr;
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(pDWFactory),
		reinterpret_cast<IUnknown**>(&pDWFactory)
	);
}

jaw::D2DGraphics::~D2DGraphics() {
	for (auto& [n, p] : bitmaps) {
		//p->pBitmap->Release();
		delete p;
	}
	bitmaps.clear();

	for (auto& [f, p] : fonts)
		p->Release();
	fonts.clear();

	pDWFactory->Release();
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
		layersChanged[i] = false;
	}
}

void jaw::D2DGraphics::EndFrame() {
	for (auto x : layers)
		x->EndDraw();

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(backgroundColor));

	D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	if (ceilf(scale) == scale) mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

	for (int i = 0; i < LAYERS; i++) {
		//always draw layer zero
		if (i && !layersChanged[i]) continue;

		ID2D1Bitmap* pBitmap = nullptr;
		layers[i]->GetBitmap(&pBitmap);
		if (!pBitmap) continue;

		pRenderTarget->DrawBitmap(
			pBitmap,
			D2D1::Rect(
				0.f,
				0.f,
				(float)ScaleUp(sizeX, scale),
				(float)ScaleUp(sizeY, scale)
			),
			1.0,
			mode
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

jaw::Point jaw::D2DGraphics::D2DBitmap::getSize() {
	return jaw::Point(x, y);
}

jaw::Bitmap* jaw::D2DGraphics::LoadBmp(std::string filename) {
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

bool jaw::D2DGraphics::DrawBmp(std::string filename, Rect dest, uint8_t layer, float alpha, bool interpolation) {
	if (!bitmaps.count(filename))
		if (!LoadBmp(filename)) return false;

	return DrawBmp(bitmaps[filename], dest, layer, alpha, interpolation);
}

bool jaw::D2DGraphics::DrawBmp(std::string filename, Point dest, uint8_t layer, float scale, float alpha, bool interpolation) {
	if (!bitmaps.count(filename))
		if (!LoadBmp(filename)) return false;

	return DrawBmp(bitmaps[filename], dest, layer, scale, alpha, interpolation);
}

bool jaw::D2DGraphics::DrawBmp(Bitmap* bmp, Point dest, uint8_t layer, float scale, float alpha, bool interpolation) {
	if (!bmp) return false;
	
	jaw::Rect destRect(
		dest.x,
		dest.y,
		dest.x + (int16_t)(bmp->getSize().x * scale),
		dest.y + (int16_t)(bmp->getSize().y * scale)
	);

	return DrawBmp(bmp, destRect, layer, alpha, interpolation);
}

bool jaw::D2DGraphics::DrawBmp(Bitmap* bmp, Rect dest, uint8_t layer, float alpha, bool interpolation) {
	if (!bmp) return false;

	D2DBitmap* pBitmap = (D2DBitmap*)bmp;

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->DrawBitmap(
		pBitmap->pBitmap,
		D2D1::Rect(
			(float)dest.tl.x,
			(float)dest.tl.y,
			(float)dest.br.x,
			(float)dest.br.y
		),
		alpha,
		interpolation ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
	);

	layersChanged[layer] = true;
	return true;
}

bool jaw::D2DGraphics::DrawPartialBmp(std::string filename, Rect dest, Rect src, uint8_t layer, float alpha, bool interpolation) {
	if (!bitmaps.count(filename))
		if (!LoadBmp(filename)) return false;

	return DrawPartialBmp(bitmaps[filename], dest, src, layer, alpha, interpolation);
}

bool jaw::D2DGraphics::DrawPartialBmp(std::string filename, Point dest, Rect src, uint8_t layer, float scale, float alpha, bool interpolation) {
	if (!bitmaps.count(filename))
		if (!LoadBmp(filename)) return false;

	return DrawPartialBmp(bitmaps[filename], dest, src, layer, scale, alpha, interpolation);
}

bool jaw::D2DGraphics::DrawPartialBmp(Bitmap* bmp, Point dest, Rect src, uint8_t layer, float scale, float alpha, bool interpolation) {
	Rect destRect(
		dest.x,
		dest.y,
		dest.x + (int16_t)(bmp->getSize().x * scale),
		dest.y + (int16_t)(bmp->getSize().y * scale)
	);

	return DrawPartialBmp(bmp, destRect, src, layer, alpha, interpolation);
}

bool jaw::D2DGraphics::DrawPartialBmp(Bitmap* bmp, Rect dest, Rect src, uint8_t layer, float alpha, bool interpolation) {
	if (!bmp) return false;
	
	D2DBitmap* pBitmap = (D2DBitmap*)bmp;

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->DrawBitmap(
		pBitmap->pBitmap,
		D2D1::Rect(
			(float)dest.tl.x,
			(float)dest.tl.y,
			(float)dest.br.x,
			(float)dest.br.y
		),
		alpha,
		interpolation ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		D2D1::Rect(
			(float)src.tl.x,
			(float)src.tl.y,
			(float)src.br.x,
			(float)src.br.y
		)
	);

	layersChanged[layer] = true;
	return true;
}

bool jaw::D2DGraphics::DrawSprite(Sprite* sprite) {
	return DrawSprite(*sprite);
}

bool jaw::D2DGraphics::DrawSprite(const Sprite& sprite) {
	if (sprite.hidden) return true;
	if (!sprite.bmp) return true;

	if (!bitmaps.count(sprite.bmp->getName()))
		if (!LoadBmp(sprite.bmp->getName())) return false;

	D2DBitmap* pBitmap = bitmaps[sprite.bmp->getName()];

	auto layer = sprite.layer;
	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	auto src = sprite.src;
	uint16_t width = src.br.x - src.tl.x;
	src.tl.x += width * sprite.frame;
	src.br.x += width * sprite.frame;

	auto mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
	if (ceilf(sprite.scale) == sprite.scale)
		mode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

	pBitmapTarget->DrawBitmap(
		pBitmap->pBitmap,
		D2D1::Rect(
			sprite.x,
			sprite.y,
			sprite.x + ((sprite.src.br.x - sprite.src.tl.x) * sprite.scale),
			sprite.y + ((sprite.src.br.y - sprite.src.tl.y) * sprite.scale)
		),
		1.f,
		mode,
		D2D1::Rect(
			(float)src.tl.x,
			(float)src.tl.y,
			(float)src.br.x,
			(float)src.br.y
		)
	);

	layersChanged[layer] = true;
	return true;
}

/*
	DIRECTWRITE
*/

bool jaw::D2DGraphics::LoadFont(const Font& font) {
	if (fonts.count(font)) return true;

	IDWriteTextFormat* pFormat = nullptr;
	auto hr = pDWFactory->CreateTextFormat(
		font.name.c_str(),
		NULL,
		font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
		font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		font.size,
		locale.c_str(),
		&pFormat
	);
	if (!pFormat || !SUCCEEDED(hr)) return false;

	switch (font.align) {
	case jaw::Font::LEFT:
		pFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;

	case jaw::Font::RIGHT:
		pFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;

	case jaw::Font::CENTER:
		pFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	}

	fonts[font] = pFormat;
	return true;
}

bool jaw::D2DGraphics::DrawString(std::wstring str, Rect dest, uint8_t layer, const Font& font, uint32_t color, float alpha) {
	if (!fonts.count(font))
		if (!LoadFont(font)) return false;

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pSolidBrush->SetColor(D2D1::ColorF(color));
	pSolidBrush->SetOpacity(alpha);

	pBitmapTarget->DrawText(
		str.c_str(),
		(UINT32)str.length(),
		fonts[font],
		D2D1::Rect(
			(float)dest.tl.x,
			(float)dest.tl.y,
			(float)dest.br.x,
			(float)dest.br.y
		),
		pSolidBrush
	);

	layersChanged[layer] = true;
	return true;
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

	layersChanged[layer] = true;
}

void jaw::D2DGraphics::FillRect(Rect dest, uint32_t color, uint8_t layer, float alpha) {
	pSolidBrush->SetColor(D2D1::ColorF(color));
	pSolidBrush->SetOpacity(alpha);

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->FillRectangle(
		D2D1::Rect(
			(float)dest.tl.x,
			(float)dest.tl.y,
			(float)dest.br.x,
			(float)dest.br.y
		),
		pSolidBrush
	);

	layersChanged[layer] = true;
}

void jaw::D2DGraphics::DrawLine(Point start, Point end, float width, uint32_t color, uint8_t layer, float alpha) {
	pSolidBrush->SetColor(D2D1::ColorF(color));
	pSolidBrush->SetOpacity(alpha);

	if (layer >= LAYERS) layer = LAYERS - 1;
	auto pBitmapTarget = layers[layer];

	pBitmapTarget->DrawLine(
		D2D1::Point2((float)start.x, (float)start.y),
		D2D1::Point2((float)end.x, (float)end.y),
		pSolidBrush,
		width
	);

	layersChanged[layer] = true;
}