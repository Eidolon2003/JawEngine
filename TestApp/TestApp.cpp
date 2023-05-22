#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:
	unsigned x;

	MyApp() {
		x = 0;
	}

	void Loop() override {
		std::cout << pWindow->getLifetime().count() << '\n';
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;
	ap.framerate = 10;

	jaw::StartEngine(new MyApp, ap, ep);

	return 0;
}