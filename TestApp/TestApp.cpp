#include "../jawengine/JawEngine.h"
#include <iostream>

class Fire : public jaw::Sprite {
public:
    Fire(jaw::Bitmap *bmp) {
        x = y = 50;
        dx = dy = 0;
        scale = 3.0f;
        this->bmp = bmp;
        src = jaw::Rect(0, 0, 50, 50);
        animationTiming = std::chrono::milliseconds(200);
    }
};


class Hello final : public jaw::AppInterface {
public:
    void Init() override {
        jaw::Bitmap* bmp = graphics->LoadBmp("fire.png");
        window->RegisterSprite(new Fire(bmp));
    }

    void Loop() override { /*We don’t need this for now*/ }
};

int main() {
    jaw::AppProperties ap;
    jaw::EngineProperties ep;

    jaw::StartEngine(new Hello, ap, ep);
}
