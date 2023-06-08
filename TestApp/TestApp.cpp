#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Init() override {
		pInput->BindKeyDown(jaw::A, [this]() {
			std::cout << pWindow->getLifetime() << '\n';
		});
	}

	void Loop() override {
		
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;
	ap.framerate = 100;
	ap.enableKeyRepeat = false;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}