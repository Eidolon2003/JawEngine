#pragma once

#include "JawEngine.h"
#include "window.h"

#include <atomic>
#include <thread>
#include <unordered_map>

namespace jaw {

	class Engine : public EngineInterface {
	public:
		Engine(const EngineProperties&);
		void OpenWindow(AppInterface*, const AppProperties&) override;
		void CloseWindow(AppInterface*) override;
		void ShowCMD(bool) override;
		std::wstring getLocale() override;

	private:
		std::unordered_map<AppInterface*, Window*> pWindows;
		std::wstring locale;

		friend void StartEngine(AppInterface*, const AppProperties&, const EngineProperties&);
	};

};