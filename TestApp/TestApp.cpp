#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Loop() override {
		std::cout << pInput->getString();
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