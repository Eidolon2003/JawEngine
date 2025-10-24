#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"

static void loop(jaw::properties *props) {}

int main() {
	jaw::properties props;
	engine::start(&props, nullptr, nullptr, loop);
}
