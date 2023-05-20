#include "engine.h"

void jaw::StartEngine(jaw::AppInterface* pApp, const jaw::AppProperties& appProps, const jaw::EngineProperties& engProps) {
	jaw::Engine engine(engProps);
	engine.OpenWindow(pApp, appProps);

	while (!engine.pWindows.empty()) {
		auto iter = engine.pWindows.begin();
		while (iter != engine.pWindows.end()) {
			if (iter->second->finished.load()) {
				iter = engine.pWindows.erase(iter);
			}
			else 
				iter++;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

jaw::Engine::Engine(const EngineProperties& props) {

#if defined WINDOWS
	HWND console = GetConsoleWindow();
	if (props.showCMD)
		ShowWindow(console, SW_SHOW);
	else
		ShowWindow(console, SW_HIDE);
#endif

}

void jaw::Engine::OpenWindow(AppInterface* pApp, const AppProperties& pProps) {
	pWindows[pApp] = new Window(pApp, this);
}

void jaw::Engine::CloseWindow(AppInterface* pApp) {

#if defined WINDOWS
	PostMessage(pWindows[pApp]->hWnd, WM_CLOSE, NULL, NULL);
#endif

}