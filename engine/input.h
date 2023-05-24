#pragma once

#include "JawEngine.h"

namespace jaw {

	class Input : public jaw::InputInterface {
	public:
		Input();

		std::pair<int, int> mouseXY;
		std::pair<int, int> getMouseXY() override;
	};

};