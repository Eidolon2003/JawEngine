#pragma once

#include "JawEngine.h"

namespace jaw {

	class Input : public jaw::InputInterface {
	public:
		Input();

		std::pair<int, int> mouseXY;
		std::pair<int, int> getMouseXY() override;

		std::string charInput;
		std::string getString() override;

		uint16_t keybits[16];
		bool isKeyPressed(uint8_t) override;
	};

};