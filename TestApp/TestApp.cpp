#include <iostream>
#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	jaw::Sprite* testSprite = nullptr;

	void Init() override {
		pGraphics->DrawString(L"Hello, world!", jaw::Rect(10,10,640,480), 0);
		pGraphics->DrawBmp("D:/Users/julia/Desktop/red.bmp", {300,10}, 0, 0.1f);

		testSprite = new jaw::Sprite;
		testSprite->bmp = "D:/Users/julia/Desktop/red.bmp";
		testSprite->src = jaw::Rect(100, 100, 200, 200);
		testSprite->dy = 1;
		pWindow->RegisterSprite(testSprite);
	}

	void Loop() override {

	}

};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = false;
	ep.locale = L"en-us";

	jaw::AppProperties ap;
	ap.sizeX = 640;
	ap.sizeY = 480;
	ap.framerate = 100;
	ap.enableKeyRepeat = false;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}