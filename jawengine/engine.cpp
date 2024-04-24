#include "engine.h"

void jaw::StartEngine(jaw::AppInterface* app, const jaw::AppProperties& appProps, const jaw::EngineProperties& engProps) {
	jaw::Engine engine(engProps);
	engine.OpenWindow(app, appProps);

	while (!engine.windows.empty()) {
		auto iter = engine.windows.begin();
		while (iter != engine.windows.end()) {
			if (iter->second->isClosed()) {
				delete iter->second;
				iter = engine.windows.erase(iter);
			}
			else {
				iter++;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

jaw::Engine::Engine(const EngineProperties& props) {
	ShowCMD(props.showCMD);
	this->locale = props.locale;
}

void jaw::Engine::OpenWindow(AppInterface* app, const AppProperties& props) {
	windows[app] = new Window(app, props, this);
}

void jaw::Engine::CloseWindow(AppInterface* app) {
#if defined WINDOWS
	PostMessage(windows[app]->getHWND(), WM_CLOSE, NULL, NULL);
#endif
}

void jaw::Engine::ShowCMD(bool show) {
#if defined WINDOWS
	HWND console = GetConsoleWindow();
	ShowWindow(console, show ? SW_SHOW : SW_HIDE);
#endif
}

std::wstring jaw::Engine::getLocale() const {
	return locale;
}