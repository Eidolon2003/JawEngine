#include "../jawengine/JawEngine.h"

class TestApp : public jaw::AppInterface {
public:
    void Init() override {
        pGraphics->FillRect(jaw::Rect(0, 0, 200, 200), 0xFF0000, 0, 1);
        pGraphics->FillRect(jaw::Rect(100, 100, 300, 300), 0x0000FF, 1, 0.5f);
    }

    void Loop() override {}
};

int main() {
    jaw::EngineProperties ep;
    jaw::AppProperties ap;
    ap.layerCount = 2;
    ap.backgroundCount = 2;

    jaw::StartEngine(new TestApp, ap, ep);
}