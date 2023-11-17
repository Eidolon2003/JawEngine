#include "../jawengine/JawEngine.h"
#include <iostream>

class MyApp : public jaw::AppInterface {

	jaw::Point end;

public:
	void Init() override {
		pInput->BindClickDown(std::bind_front(&MyApp::ClickDown, this));

		end = jaw::Point(-1, -1);
	}

	void Loop() override {
		if (end.x < 0) return;

		jaw::Point begin(pWindow->getProperties().size / 2);
		pGraphics->DrawLine(begin, end, 2.f, 0xFFFFFF, 1);
	}

	void ClickDown(jaw::InputInterface::Mouse mouse) {
		if (mouse.lmb) end = mouse.pos;
	}
};

int main() {
	jaw::EngineProperties ep;
	jaw::AppProperties ap;
	ap.framerate = 10;
	ap.size = { 800,600 };

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}