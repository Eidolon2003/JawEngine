#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"

#include <cstdlib>
#include <math.h>

size_t makeSine(float freq, jaw::nanoseconds length, int16_t** data) {
	constexpr int SAMPLE_RATE = 44100;
	constexpr float AMPLITUDE = 0.5f;

	float seconds = jaw::to_seconds(length);
	size_t numSamples = (size_t)(SAMPLE_RATE * seconds);
	size_t totalSamples = numSamples * 2;

	*data = (int16_t*)malloc(totalSamples * 2);
	if (!*data) return 0;

	for (size_t i = 0; i < numSamples; i++) {
		float t = (float)i / SAMPLE_RATE;
		float sample = sinf(2*PI32 * freq * t);
		int16_t scaledSample = (int16_t)(sample * AMPLITUDE * 32767);
		(*data)[i * 2] = scaledSample;
		(*data)[i * 2 + 1] = scaledSample;
	}

	return totalSamples * 2;
}

static void init(jaw::properties* props) {
	int16_t* sine;
	size_t len = makeSine(440.f, jaw::seconds(1), &sine);
	if (sine) {
		auto id = sound::create();
		sound::write(id, sine, len, false);
		sound::start(id);
	}
}

static void loop(jaw::properties* props) {}

int main() {
	jaw::properties props;
	engine::start(&props, nullptr, init, loop);
}
