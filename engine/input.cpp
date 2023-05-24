#include "input.h"

jaw::Input::Input() {
	mouseXY = { 0, 0 };
}

std::pair<int, int> jaw::Input::getMouseXY() {
	return mouseXY;
}