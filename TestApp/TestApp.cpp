#include "TestApp.h"

int main() {
	jaw::Engine engine;
	engine.OpenWindow(new MyApp);
	for (;;) std::this_thread::sleep_for(std::chrono::seconds(1));
	return 0;
}

void MyApp::Loop() {
	std::cout << "Hello\n";
}