#include "../jawengine/JawEngine.h"
#include <iostream>

class MySprite : public jaw::Sprite {
public:
	MySprite() {
		x = y = 50;
		lifetime = std::chrono::seconds(10);
	}

	void Draw(jaw::AppInterface* pApp) override {
		auto rect = jaw::Rect(x, y, x + 50, y + 50);
		pApp->pGraphics->FillRect(rect, 0xC0FFEE, layer);
	}
};

class MyApp : public jaw::AppInterface {
public:

	std::weak_ptr<MySprite> sprite;

	void Init() override {
		sprite = pWindow->RegisterSprite(new MySprite);
	}

	void Loop() override {
		if (!sprite.expired())
			sprite.lock()->x++;
	}
};

int main() {
	jaw::EngineProperties ep;
	ep.showCMD = false;

	jaw::AppProperties ap;
	ap.framerate = 60;
	ap.size = { 800,600 };

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}