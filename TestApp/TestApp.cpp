#include <iostream>
#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Init() override {
		pGraphics->DrawString(L"Hello, World!", jaw::Rect(1, 1, 64, 48), 0);
	}

	void Loop() override {
		auto mouse = pInput->getMouse();
		std::cout << mouse.x << ", " << mouse.y << std::endl;

		pGraphics->FillRect(jaw::Rect(mouse.x, mouse.y, mouse.x + 1, mouse.y + 1), 0xFF0000, 1);
	}

};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;
	ep.locale = L"en-us";

	jaw::AppProperties ap;
	ap.title = "Custom Title!";
	ap.sizeX = 40;
	ap.sizeY = 30;
	ap.scale = 20;
	ap.framerate = 60;
	ap.enableKeyRepeat = false;

	jaw::StartEngine(new MyApp, ap, ep);

	sizeof(jaw::AppProperties);

	return 0;
}