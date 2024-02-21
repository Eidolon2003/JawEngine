#include "../jawengine/JawEngine.h"
#include <iostream>

class MySprite : public jaw::Sprite {
public:
	MySprite() {
		x = y = 50;
		lifetime = std::chrono::seconds(5);
	}

	void Draw(jaw::AppInterface* pApp) override {
		auto rect = jaw::Rect(x, y, x + 20, y + 20);
		pApp->pGraphics->FillRect(rect, 0xFFFFFF, layer);
	}
};

class MyApp : public jaw::AppInterface {
public:

	MySprite* sprite = nullptr;

	void Init() override {
		sprite = new MySprite;
		pWindow->RegisterSprite(sprite);
	}

	void Loop() override {

	}
};

int main() {
	jaw::EngineProperties ep;
	jaw::AppProperties ap;
	ap.framerate = 100;
	ap.size = { 800,600 };

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}