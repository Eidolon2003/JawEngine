#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	int x = 320, y = 240;

	void Init() override {
		pInput->BindKeyDown(jaw::ESC, std::bind(&jaw::EngineInterface::CloseWindow, pEngine, this));

		pInput->BindClickDown([this](auto mouse) {
			if (mouse.lmb) {
				x = mouse.x;
				y = mouse.y;
			}
		});
	}

	void Loop() override {
		auto mouse = pInput->getMouse();
		printf("%d, %d\t%d\n", mouse.x, mouse.y, mouse.wheel);

		pGraphics->FillRect(mouse.x, mouse.y, x, y, 0xB00B1E, 0);
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;
	ap.sizeX = 640;
	ap.sizeY = 480;
	ap.framerate = 120;
	ap.enableKeyRepeat = false;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}