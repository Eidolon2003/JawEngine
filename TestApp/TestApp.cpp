#include "../jawengine/JawEngine.h"
#include <iostream>

class TestApp : public jaw::AppInterface {
public:
	void Init() override {
		engine->ShowCMD(true);
	}

	void Loop() override {
		if (window->getLifetime() > std::chrono::seconds(5))
			engine->CloseWindow(this);

		std::cout << window->getFrametime() << std::endl;
	}
};

int main() {
	jaw::EngineProperties ep;
	jaw::AppProperties ap;
	jaw::StartEngine(new TestApp, ap, ep);
}