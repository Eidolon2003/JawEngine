#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"

static void init(jaw::properties* props) {}
static void loop(jaw::properties* props) {}

int main() {
	jaw::properties props;
	engine::start(&props, nullptr, init, loop);
}
