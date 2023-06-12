#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	static constexpr jaw::AppProperties properties = { 640, 480, 100, false };

	void Init() override {
		
	}

	void Loop() override {
		auto mouse = pInput->getMouse();
		pGraphics->DrawBmp("D:/Users/julia/Desktop/red.bmp", mouse.x, mouse.y, 1, 0.1f);
	}

};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = false;

	jaw::StartEngine(new MyApp, MyApp::properties, ep);

	return 0;
}