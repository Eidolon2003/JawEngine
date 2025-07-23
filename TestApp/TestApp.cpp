#include "../jawengine/JawEngine.h"
#include <random>

#define WIDTH 1280
#define HEIGHT 1024

#if 1
static jaw::properties props;
static draw::bmpid bmp;
static draw::bmpOptions bmpOpt{};
static draw::argb pixels[WIDTH * HEIGHT];

static std::mt19937 random(0);
static std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

void game::init() {
	bmp = draw::createBmp(jaw::vec2i(WIDTH, HEIGHT));

	bmpOpt.bmp = bmp;
	bmpOpt.src = jaw::recti(0, 0, WIDTH, HEIGHT);
	bmpOpt.dest = bmpOpt.src;
}

void game::loop() {
	//Write 64 random bits at a time
	for (int i = 0; i < WIDTH*HEIGHT/2; i++) {
		((uint64_t*)pixels)[i] = dist(random);
	}
	draw::writeBmp(bmp, pixels);
	draw::bmp(&bmpOpt, 0);

	static char fps[64];
	snprintf(fps, 64, "%d", (int)props.getFramerate());
	draw::strOptions opt{};
	opt.rect = jaw::recti(0, 0, WIDTH, HEIGHT);
	opt.color = draw::color::WHITE;
	opt.font = 0;
	opt.str = fps;
	draw::str(&opt, 1);
}
#else
jaw::properties props;
static draw::drawCall pixelCalls[WIDTH*HEIGHT];

static std::mt19937 mt(0);
static std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFF);

void game::init() {
	draw::rectOptions opt{};
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			size_t index = j * WIDTH + i;
			opt.rect = jaw::recti(i, j, i + 1, j + 1);
			pixelCalls[index] = draw::makeDraw(draw::type::RECT, 0, &opt);
		}
	}
}

void game::loop() {
	for (int i = 0; i < WIDTH*HEIGHT; i++) {
		((draw::rectOptions*)(pixelCalls[i].data))->color = dist(mt);
	}
	draw::enqueueMany(pixelCalls, WIDTH*HEIGHT);

	static char fps[64];
	snprintf(fps, 64, "%d", (int)props.getFramerate());
	draw::strOptions opt{};
	opt.rect = jaw::recti(0, 0, WIDTH, HEIGHT);
	opt.color = draw::color::WHITE;
	opt.font = 0;
	opt.str = fps;
	draw::str(&opt, 1);
}
#endif

int main() { 
	props.framerate = 0;
	props.size = jaw::vec2i(WIDTH, HEIGHT);
	props.title = "Color Snow Performance Test";
	props.mode = jaw::properties::WINDOWED;
	engine::start(&props);
}