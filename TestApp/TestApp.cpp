#include "../jawengine/JawEngine.h"

class TestApp : public jaw::AppInterface {
public:
    int x = 0;

    void Init() override {
        pGraphics->FillRect(jaw::Rect(100, 100, 300, 300), 0x0000FF, 1, 0.5f);
        pGraphics->FillRect(jaw::Rect(0, 0, 200, 200), 0xFF0000, 0, 1);
    }

    void Loop() override {
        pGraphics->FillRect(jaw::Rect(x, 10, x + 10, 20), 0x00FF00, 2, 0.5f);
        x++;
    }
};

int main() {
    jaw::EngineProperties ep;
    jaw::AppProperties ap;
    ap.layerCount = 3;
    ap.backgroundCount = 2;

    jaw::StartEngine(new TestApp, ap, ep);
}