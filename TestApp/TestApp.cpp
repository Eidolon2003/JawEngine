#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:
	std::wstring text;

	void Init() override {
		pInput->BindKeyDown(jaw::ESC, [&]() {pEngine->CloseWindow(this); });
	}

	void Loop() override {
		auto oldlen = text.length();
		text.append(pInput->getString());

		for (;;) {
			auto pos = text.find(L'\b', oldlen);
			if (pos == std::wstring::npos) break;
			
			if (pos == 0)
				text.erase(0, 1);
			else
				text.erase(pos - 1, 2);
		}
		
		auto halfSeconds = pWindow->getLifetime().count() / 500;
		auto output = halfSeconds % 2 ? text : text + L'_';
		pGraphics->DrawString(output, jaw::Rect(2, 2, 640, 480), 1);
		pGraphics->DrawString(std::to_wstring(text.length()), jaw::Rect(2, 465, 640, 480), 1);
	}

};

int main() {
	jaw::EngineProperties ep;

	jaw::AppProperties ap;
	ap.title = "Simple Text Input";

	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}