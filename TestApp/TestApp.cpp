#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:
	void Init() override {

	}

	void Loop() override {
		double framerate = 1 / pWindow->getFrametime().count() * 1000;
		pGraphics->DrawString(std::to_wstring((unsigned)framerate), jaw::Rect(0, 0, 640, 480), 1);
	}

};

int main() {
	jaw::EngineProperties ep;
	jaw::AppProperties ap;
	ap.framerate = 0;

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}