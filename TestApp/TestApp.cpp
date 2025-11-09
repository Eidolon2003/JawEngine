#include "../jawengine/JawEngine.h"

#define DEADZONE 0.05

struct player {
	jaw::sprid spr = jaw::INVALID_ID;
	float acceleration = 4000;
	float friction = 5;
	int16_t rad = 10;
};
static player p;

jaw::fontid font = jaw::INVALID_ID;

static void drawPlayer(jaw::sprid id, jaw::properties *props) {
	jaw::sprite *spr = sprite::idtoptr(id);
	if (!spr) return;

	draw::enqueue(draw::rect{
		.rect = jaw::recti(spr->pos-p.rad, spr->pos+p.rad+1),
		.color = jaw::color::WHITE
	}, spr->z);
}

// This causes a big stutter every five seconds, but it demonstrates the timer
static void checkGamepads(jaw::properties *props) {
	input::findNewGamepads();
	util::setTimer(props, jaw::seconds(5), checkGamepads);
}

static void init(jaw::properties *props) {
	sprite::clear();
	p.spr = sprite::create(jaw::sprite{
		.pos = props->size / 2,
		.z = 1,
	});
	sprite::customDraw(p.spr, drawPlayer);

	font = draw::newFont(draw::font{
		.name = "Comic Sans MS",
		.size = 30
	});

	util::setTimer(props, jaw::seconds(5), checkGamepads);
}

static void loop(jaw::properties *props) {
	auto ctrl = input::getGamepad(0);
	if (!ctrl) return;

	auto leftstick = ctrl->sony.l;
	if (fabs(leftstick.x) < DEADZONE) leftstick.x = 0;
	if (fabs(leftstick.y) < DEADZONE) leftstick.y = 0;
	if (fabs(leftstick.x) > 1-DEADZONE) leftstick.x = roundf(leftstick.x);
	if (fabs(leftstick.y) > 1-DEADZONE) leftstick.y = roundf(leftstick.y);

	auto spr = sprite::idtoptr(p.spr);
	if (!spr) return;

	jaw::vec2f acc = leftstick * p.acceleration;
	spr->vel = spr->vel + (acc * jaw::to_seconds(props->totalFrametime));
	spr->vel = spr->vel - (spr->vel * jaw::to_seconds(props->totalFrametime) * p.friction);

	float fps = 1 / jaw::to_seconds(props->totalFrametime);
	char *buf = util::tempalloc<char>(16);
	snprintf(buf, 16, "%.0f fps", fps);
	draw::enqueue(draw::str{
		.rect = jaw::recti(0, props->size),
		.str = buf,
		.color = jaw::color::WHITE,
		.font = font
	}, 3);

	if (ctrl->sony.x.isDown) engine::stop();
}

int main() {
	std::vector<asset::INIEntry> vec;
	vec.emplace_back("width", "1280", "Window width in pixels");
	vec.emplace_back("height", "1024", "Window height in pixels");
	asset::ini("F:/assets/ini/test.ini", &vec);

	jaw::properties props;
	props.size = jaw::vec2i(
		std::stoi(vec[0].value),
		std::stoi(vec[1].value)
	);

	props.targetFramerate = 0;

	engine::start(&props, nullptr, init, loop);
}