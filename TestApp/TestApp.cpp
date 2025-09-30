#define _CRT_SECURE_NO_WARNINGS
#include "../jawengine/JawEngine.h"

jaw::bmpid bmp;
jaw::argb* img;
jaw::vec2i dim;

static void init(jaw::properties* props) {
	dim = asset::bmp("F:/assets/test-animation/idle-40x70x6.png", &img);
	bmp = draw::createBmp(dim);
	draw::writeBmp(bmp, img, dim);
}

static void loop(jaw::properties* props) {
	draw::bmp(draw::bmpOptions{
		.bmp = bmp,
		.src = jaw::recti(0,dim),
		.dest = jaw::recti(0,dim)
	}, 0);
}

int main() {
	jaw::properties props;
	props.scale = 4;
	engine::start(&props, nullptr, init, loop);
}
