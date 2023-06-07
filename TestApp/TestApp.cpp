#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	void Loop() override {
		if (pInput->isKeyPressed(jaw::A))
			std::cout << "Yes A\n";
		else
			std::cout << "No A\n";

		if (pInput->isKeyPressed(jaw::B))
			std::cout << "Yes B\n";
		else
			std::cout << "No B\n";
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