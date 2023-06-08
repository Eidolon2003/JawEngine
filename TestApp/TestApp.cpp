#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Init() override {
		pInput->BindKeyDown(jaw::ESC, std::bind(&jaw::EngineInterface::CloseWindow, pEngine, this));
	}

	void Loop() override {
		auto mouse = pInput->getMouse();
		printf("LMB: %u\n", !!mouse.lmb);
		printf("RMB: %u\n", !!mouse.rmb);
		printf("MMB: %u\n", !!mouse.mmb);
		printf("XMB1: %u\n", !!mouse.xmb1);
		printf("XMB2: %u\n", !!mouse.xmb2);
		printf("SHIFT: %u\n", !!mouse.shift);
		printf("CTRL: %u\n", !!mouse.ctrl);
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;
	ap.framerate = 5;
	ap.enableKeyRepeat = false;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}