#include "input.h"

jaw::Input::Input() {
	mouseXY = { 0, 0 };
	charInput = "";
}

std::pair<int, int> jaw::Input::getMouseXY() {
	return mouseXY;
}

std::string jaw::Input::getString() {
	return charInput;
}