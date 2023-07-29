#include "../jawengine/JawEngine.h"
#include <iostream>

class MyApp : public jaw::AppInterface {
public:
	void Init() override {
		pInput->BindClickDown(std::bind_front(&MyApp::ClickDown, this));
	}

	void Loop() override {
		
	}

	void ClickDown(jaw::InputInterface::Mouse mouse) {
		if (mouse.lmb) std::cout << "click\n";
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;
	jaw::AppProperties ap;
	ap.framerate = 50;

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}