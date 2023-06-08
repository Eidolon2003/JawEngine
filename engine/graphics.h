#pragma once

#include "JawEngine.h"
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>

namespace jaw {

	class D2DGraphics : public GraphicsInterface {
	private:
		ID2D1HwndRenderTarget* pRenderTarget;
		std::vector<ID2D1BitmapRenderTarget*> layers;
		ID2D1Factory* pD2DFactory;
		ID2D1SolidColorBrush* pSolidBrush;

		HWND hWnd;
		uint16_t sizeX, sizeY;

	public:
		D2DGraphics(HWND hWnd, uint16_t x, uint16_t y);
		~D2DGraphics();

		void BeginFrame();
		void EndFrame();

		void setSize(uint16_t x, uint16_t y);

		void FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, uint8_t layer) override;
	};

};