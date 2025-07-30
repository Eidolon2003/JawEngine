#include "input.h"
#include "internal_input.h"
#include <string>	//memcpy

static jaw::mouse mouse;

void input::updateMouse(const jaw::mouse* m) {
	memcpy(&mouse, m, sizeof(jaw::mouse));
}

const jaw::mouse* input::getMouse() {
	return &mouse;
}