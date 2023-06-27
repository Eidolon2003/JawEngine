#include <iostream>
#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Init() override {
		auto testSprite = new jaw::Sprite();
		testSprite->bmp = pGraphics->LoadBmp("D:/Users/julia/Pictures/red.bmp");
		testSprite->src = jaw::Rect(0, 0, 50, 50);
		testSprite->animationTiming = std::chrono::milliseconds(50);
		testSprite->lifetime = std::chrono::seconds(5);
		pWindow->RegisterSprite(testSprite);

		testSprite = new jaw::Sprite();
		testSprite->bmp = pGraphics->LoadBmp("D:/Users/julia/Pictures/red.bmp");
		testSprite->src = jaw::Rect(0, 0, 50, 50);
		testSprite->y = 100;
		testSprite->dx = 5;
		pWindow->RegisterSprite(testSprite);

		pGraphics->DrawBmp("D:/Users/julia/Pictures/red.bmp", jaw::Point(200, 200), 0);
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