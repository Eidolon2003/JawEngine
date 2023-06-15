#include <iostream>
#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	jaw::GraphicsInterface::Font font;

	void Init() override {
		pGraphics->DrawString(L"Hello, world!", 10, 10, 0, font);
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