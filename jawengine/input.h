#pragma once

#include "JawEngine.h"

namespace jaw {

	class Input : public jaw::InputInterface {
	public:
		Input(bool repeat);
		void Reset();

		Mouse mouse;
		Mouse getMouse() const override;

		std::wstring charInput;
		std::wstring getString() const override;

		bool enableKeyRepeat;

		uint16_t keybits[16];
		bool isKeyPressed(uint8_t) const override;

		static constexpr int TABLELEN = 256;
		std::function<void()> downJumpTable[TABLELEN];
		std::function<void()> upJumpTable[TABLELEN];
		void BindKeyDown(uint8_t, const std::function<void()>&) override;
		void BindKeyUp(uint8_t, const std::function<void()>&) override;

		std::function<void(Mouse)> clickDown;
		std::function<void(Mouse)> clickUp;
		void BindClickDown(const std::function<void(Mouse)>&) override;
		void BindClickUp(const std::function<void(Mouse)>&) override;
	};

};