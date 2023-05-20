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

		if (x == 40) pEngine->OpenWindow(new MyApp, {});
		if (x == 50) pEngine->CloseWindow(this);
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = true;

	jaw::AppProperties ap;

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}