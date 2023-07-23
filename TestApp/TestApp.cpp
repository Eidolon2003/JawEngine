#include "../jawengine/JawEngine.h"

class MySprite : public jaw::Sprite {
	void Draw(jaw::AppInterface* p) final override {
		p->pGraphics->FillRect(jaw::Rect((uint16_t)x, (uint16_t)y, (uint16_t)x + 20, (uint16_t)y + 20), 0xFFFFFF, layer);
	}

public:
	MySprite(float xy) {
		x = xy; y = xy;
	}
};

class MyApp : public jaw::AppInterface {
public:
	void Init() override {
		auto spr = new MySprite(50);
		spr->lifetime = std::chrono::seconds(2);
		spr->dx = 10;
		pWindow->RegisterSprite(spr);
		pWindow->RegisterSprite(new MySprite(100));
	}

	void Loop() override {
		double framerate = 1 / pWindow->getFrametime().count() * 1000;
		pGraphics->DrawString(std::to_wstring((unsigned)framerate), jaw::Rect(0, 0, 640, 480), 1);
	}

};

int main() {
	jaw::EngineProperties ep;
	jaw::AppProperties ap;
	ap.framerate = 50;

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}