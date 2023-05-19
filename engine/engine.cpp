#include "engine.h"

void WindowThreadFunk(jaw::Application* pApp) {
	Window window(pApp);

	bool running = true;
	while (running) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		window.pApp->Loop();

		Sleep(100);
	}
}

jaw::Engine::Engine() {

}

void jaw::Engine::OpenWindow(Application* pApp) {
	auto pThread = new std::thread(WindowThreadFunk, pApp);
	pWindowThreads[pApp] = pThread;
	pThread->detach();
}

void jaw::Engine::CloseWindow(Application* pApp) {

}