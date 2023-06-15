#pragma once

#include "graphics.h"
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>
#include <unordered_map>
#include <map>

namespace jaw {

	class D2DGraphics : public InternalGraphicsInterface {
	public:
		class D2DBitmap : public Bitmap {
		public:
			D2DBitmap(std::string, ID2D1Bitmap*);

			ID2D1Bitmap* pBitmap;

			std::string name;
			std::string getName() override;

			uint32_t x, y;
			std::pair<uint32_t, uint32_t> getSize() override;
		};

	private:
		ID2D1HwndRenderTarget* pRenderTarget;
		ID2D1Factory* pD2DFactory;
		ID2D1SolidColorBrush* pSolidBrush;
		IDWriteFactory* pDWFactory;

		static constexpr uint8_t LAYERS = 16;
		std::vector<ID2D1BitmapRenderTarget*> layers;

		std::unordered_map<std::string, D2DBitmap*> bitmaps;

		struct CmpFont {
			bool operator()(const Font& lhs, const Font& rhs) const {
				//return lhs < rhs;
				if (lhs.name != rhs.name)
					return lhs.name < rhs.name;

				if (lhs.bold != rhs.bold)
					return lhs.bold < rhs.bold;

				if (lhs.italic != rhs.italic)
					return lhs.bold < rhs.bold;

				return lhs.size < rhs.size;
			}
		};
		std::map<Font, IDWriteTextFormat*, CmpFont> fonts;

		HWND hWnd;
		uint16_t sizeX, sizeY;
		std::wstring locale;
		uint32_t backgroundColor;

	public:
		D2DGraphics(HWND hWnd, uint16_t x, uint16_t y, std::wstring locale);
		~D2DGraphics() override;

		void BeginFrame() override;
		void EndFrame() override;
		void setSize(uint16_t x, uint16_t y) override;

		Bitmap* LoadBmp(std::string filename) override;
		bool DrawBmp(std::string filename, uint16_t x, uint16_t y, uint8_t layer, float scale = 1.f, float opacity = 1.f, bool interpolation = false) override;

		bool LoadFont(Font& font) override;
		bool DrawString(std::wstring str, uint16_t x, uint16_t y, uint8_t layer, Font& font, uint32_t color = 0xFFFFFF) override;

		void setBackgroundColor(uint32_t color) override;
		void ClearLayer(uint8_t layer, uint32_t color = 0x000000, float alpha = 0.f) override;
		void FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, uint8_t layer) override;
	};

};