#include "engine.h"

void jaw::StartEngine(jaw::AppInterface* pApp, const jaw::AppProperties& appProps, const jaw::EngineProperties& engProps) {
	jaw::Engine engine(engProps);
	engine.OpenWindow(pApp, appProps);

	while (!engine.pWindows.empty()) {
		auto iter = engine.pWindows.begin();
		while (iter != engine.pWindows.end()) {
			if (iter->second->isClosed()) {
				delete iter->second;
				iter = engine.pWindows.erase(iter);
			}
			else 
				iter++;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

jaw::Engine::Engine(const EngineProperties& props) {
	ShowCMD(props.showCMD);
}

void jaw::Engine::OpenWindow(AppInterface* pApp, const AppProperties& props) {
	pWindows[pApp] = new Window(pApp, props, this);
}

void jaw::Engine::CloseWindow(AppInterface* pApp) {
#if defined WINDOWS
	PostMessage(pWindows[pApp]->getHWND(), WM_CLOSE, NULL, NULL);
#endif
}

void jaw::Engine::ShowCMD(bool show) {
#if defined WINDOWS
	HWND console = GetConsoleWindow();
	if (show)
		ShowWindow(console, SW_SHOW);
	else
		ShowWindow(console, SW_HIDE);
#endif
}