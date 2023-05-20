#pragma once

#include "JawEngine.h"
#include "window.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <unordered_map>

namespace jaw {

	class Engine : public EngineInterface {
	public:
		Engine(const EngineProperties&);
		void OpenWindow(AppInterface*, const AppProperties&) override;
		void CloseWindow(AppInterface*) override;

	private:
		std::unordered_map<AppInterface*, Window*> pWindows;

		friend void StartEngine(AppInterface*, const AppProperties&, const EngineProperties&);
	};

};