#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Init() override {
		pInput->BindKeyDown(jaw::ESC, std::bind(&jaw::EngineInterface::CloseWindow, pEngine, this));
	}

	void Loop() override {
		auto mouse = pInput->getMouse();
		printf("%d, %d\n", mouse.x, mouse.y);

		pGraphics->FillRect(50, 50, 350, 350, 0xF01010, 0);
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;
	ap.sizeX = 640;
	ap.sizeY = 480;
	ap.framerate = 100;
	ap.enableKeyRepeat = false;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}