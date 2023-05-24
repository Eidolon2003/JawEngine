#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:
	unsigned x;

	MyApp() {
		x = 0;
	}

	void Loop() override {
		auto mouse = pInput->getMouseXY();
		printf("%d,%d\n", mouse.first, mouse.second);
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;
	ap.framerate = 100;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}