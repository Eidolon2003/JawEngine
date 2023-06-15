#include <iostream>
#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Init() override {
		pGraphics->DrawString(L"Hello, world!", jaw::Rect(10,10,640,480), 0);
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