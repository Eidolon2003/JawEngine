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

			uint16_t x, y;
			Point getSize() override;
		};

	private:
		ID2D1HwndRenderTarget* pRenderTarget;
		ID2D1Factory* pD2DFactory;
		ID2D1SolidColorBrush* pSolidBrush;
		IDWriteFactory* pDWFactory;

		uint8_t layerCount;
		uint8_t backgroundCount;
		std::vector<ID2D1BitmapRenderTarget*> layers;
		std::vector<bool> layersChanged;

		std::unordered_map<std::string, D2DBitmap*> bitmaps;

		struct CmpFont {
			bool operator()(const Font& lhs, const Font& rhs) const {
				//return lhs < rhs;
				if (lhs.name != rhs.name)
					return lhs.name < rhs.name;

				if (lhs.bold != rhs.bold)
					return lhs.bold < rhs.bold;

				if (lhs.italic != rhs.italic)
					return lhs.italic < rhs.italic;

				if (lhs.align != rhs.align)
					return lhs.align < rhs.align;

				return lhs.size < rhs.size;
			}
		};
		std::map<Font, IDWriteTextFormat*, CmpFont> fonts;

		HWND hWnd;
		uint16_t sizeX, sizeY;
		float scale;
		std::wstring locale;
		uint32_t backgroundColor;

	public:
		D2DGraphics(HWND hWnd, AppProperties properties, std::wstring locale);
		~D2DGraphics() override;

		void BeginFrame() override;
		void EndFrame() override;
		void setSize(uint16_t x, uint16_t y) override;

		Bitmap* LoadBmp(std::string filename) override;
		
		bool DrawBmp(std::string filename, Point dest, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) override;
		bool DrawBmp(std::string filename, Rect dest, uint8_t layer, float alpha = 1.f, bool interpolation = false) override;
		bool DrawBmp(Bitmap* bmp, Point dest, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) override;
		bool DrawBmp(Bitmap* bmp, Rect dest, uint8_t layer, float alpha = 1.f, bool interpolation = false) override;
		
		bool DrawPartialBmp(std::string filename, Rect dest, Rect src, uint8_t layer, float alpha = 1.f, bool interpolation = false) override;
		bool DrawPartialBmp(std::string filename, Point dest, Rect src, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) override;
		bool DrawPartialBmp(Bitmap* bmp, Rect dest, Rect src, uint8_t layer, float alpha = 1.f, bool interpolation = false) override;
		bool DrawPartialBmp(Bitmap* bmp, Point dest, Rect src, uint8_t layer, float scale = 1.f, float alpha = 1.f, bool interpolation = false) override;

		bool DrawSprite(Sprite* sprite) override;
		bool DrawSprite(const Sprite& sprite) override;

		bool LoadFont(const Font& font) override;
		bool DrawString(std::wstring str, Rect dest, uint8_t layer, const Font& font = Font(), uint32_t color = 0xFFFFFF, float alpha = 1.f) override;

		void setBackgroundColor(uint32_t color) override;
		void ClearLayer(uint8_t layer, uint32_t color = 0x000000, float alpha = 0.f) override;
		void FillRect(Rect dest, uint32_t color, uint8_t layer, float alpha = 1.f) override;
		void DrawLine(Point start, Point end, float width, uint32_t color, uint8_t layer, float alpha = 1.f) override;
	};

};