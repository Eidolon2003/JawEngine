#pragma once

#include "JawEngine.h"

namespace jaw {

	class Input : public jaw::InputInterface {
	public:
		Input(bool repeat);

		std::pair<int, int> mouseXY;
		std::pair<int, int> getMouseXY() override;

		std::string charInput;
		std::string getString() override;

		bool enableKeyRepeat;

		uint16_t keybits[16];
		bool isKeyPressed(uint8_t) override;

		static constexpr int TABLELEN = 256;
		std::function<void()> downJumpTable[TABLELEN];
		std::function<void()> upJumpTable[TABLELEN];
		void BindKeyDown(uint8_t, std::function<void()>) override;
		void BindKeyUp(uint8_t, std::function<void()>) override;
	};

};