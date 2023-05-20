#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:
	unsigned x;

	MyApp() {
		x = 0;
	}

	void Loop() override {
		std::cout << x++ << '\n';

		jaw::AppProperties properties;
		properties.framerate = 100;

		if (x == 5) pEngine->OpenWindow(new MyApp, properties);
		if (x == 10) pEngine->CloseWindow(this);
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