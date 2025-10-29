#include "../jawengine/JawEngine.h"

jaw::bmpid bmp;
jaw::argb *img;
jaw::vec2i dim;

int16_t *soundSamples;
jaw::soundid soundID;

static void init(jaw::properties *props) {
	img = asset::bmp("F:/assets/test-animation/idle-40x70x6.png", &dim);
	bmp = draw::createBmp(dim);
	draw::writeBmp(bmp, img, dim);

	soundID = sound::create();
	size_t numSamples;
	soundSamples = asset::wav("F:/assets/wav/win.wav", &numSamples);
	sound::write(soundID, soundSamples, numSamples, false);

	sound::start(soundID);
}

static void loop(jaw::properties *props) {
	draw::enqueue(draw::bmp{
		.bmp = bmp,
		.src = jaw::recti(0, dim),
		.dest = jaw::recti(0, dim)
		}, 0);
}

int main() {
	jaw::properties props;
	props.scale = 4;
	props.size = jaw::vec2i(320, 240);
	engine::start(&props, nullptr, init, loop);
}