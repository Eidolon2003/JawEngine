#include <iostream>
#include "../engine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:

	static constexpr jaw::AppProperties properties = { 640, 480, 10, false };

	void Init() override {
		
	}

	void Loop() override {
		pEngine->OpenWindow(new MyApp, MyApp::properties);
		pEngine->CloseWindow(this);
	}

};

int main() {

	jaw::StartEngine(new MyApp, MyApp::properties, { true });

	return 0;
}