// JawEngine 0.2.0 Snake Example

// This game does not use sprites or sound, so we can disable them.
// If the engine lib also happens to have been compiled with -DNSPRITE=ON and -DNSOUND=ON, 
// they will be left out of the lib resulting in a smaller binary
#define JAW_NSPRITE
#define JAW_NSOUND
#include "../jawengine/JawEngine.h"

#include <ctime>
#include <list>
#include <random>

/*
*	FORWARD DECLARE STATE FUNCTIONS
*/
static jaw::stateid titleState = 0;	// The state used with engine::start is ID 0
static void title_initOnce(jaw::properties*);	// A state's initOnce function is called once when it's created
static void title_init(jaw::properties*);		// A state's init function is called when it becomes active
static void title_loop(jaw::properties*);		// A state's loop function is called once every frame when active

static jaw::stateid gameState = jaw::INVALID_ID;
static void game_initOnce(jaw::properties*);
static void game_init(jaw::properties*);
static void game_loop(jaw::properties*);

/*
*	MAIN
*/
int main() {
	jaw::properties props;
	props.title = "Snake Demo";
	props.targetFramerate = 0;	// Vsync
	props.size = jaw::vec2i(1280, 1024); // 1280x1024 windowed mode
	props.enableSubpixelTextRendering = true;
	engine::start(&props, title_initOnce, title_init, title_loop);	// Start in title screen
}

/*
*	TITLE SCREEN STATE
*/
static jaw::fontid titleFont = jaw::INVALID_ID;
static jaw::fontid buttonFont = jaw::INVALID_ID;
static jaw::fontid buttonFontBold = jaw::INVALID_ID;
static auto playButtonRect = jaw::recti();
static bool controller = false;

static void playButtonHandler(jaw::properties *props) {
	// When the play button is clicked, go into the game state
	state::push(gameState);
}

// This is called once when the state is created
// Used for one-time initialization
static void title_initOnce(jaw::properties *props) {
	// Registering new fonts with the draw API
	titleFont = draw::newFont(draw::font{
		.name = "Comic Sans MS",
		.size = (float)props->size.y/8,
		.bold = true,
		.align = draw::font::CENTER
	});
	buttonFont = draw::newFont(draw::font{
		.name = "Comic Sans MS",
		.size = (float)props->size.y/32,
		.align = draw::font::CENTER
	});
	buttonFontBold = draw::newFont(draw::font{
		.name = "Comic Sans MS",
		.size = (float)props->size.y/32,
		.bold = true,
		.align = draw::font::CENTER
	});

	// Set the position of the play button based on the window size
	playButtonRect = jaw::recti(
		props->size.x/2 - props->size.x/8 - 1,
		props->size.y/2 - props->size.y/16 - 1,
		props->size.x/2 + props->size.x/8,
		props->size.y/2 + props->size.y/16
	);

	// Create the game state
	// This calls game_initOnce
	gameState = state::create(props, game_initOnce, game_init, game_loop);
}

static void title_init(jaw::properties *props) {
	draw::setBackgroundColor(jaw::color::BLACK);

	// Clear any clickables and bindings from previous states
	input::clear();
	input::bindKeyDown(key::ESC, [](jaw::properties*) { engine::stop(); });

	// Set up the play button clickable
	input::createClickable(jaw::clickable{
		// A function that returns the rect of the button
		[](jaw::properties*) { return playButtonRect; },

		// The callback that is called when the button is clicked
		playButtonHandler,

		// The mouse click conditions (left click, not shift or ctrl click)
		jaw::mouseFlags{ .lmb=true }
	});
}

static void title_loop(jaw::properties *props) {
	// Draw the title text
	draw::enqueue(draw::str{
		.rect = jaw::recti(0, props->size),
		.str = "JawEngine\nSnake",
		.color = jaw::color::WHITE,
		.font = titleFont
	}, 0);

	// Indicate when the button is hovered over
	auto mousePos = props->mouse.pos;
	bool hover = playButtonRect.contains(mousePos);
	int thickness = hover ? 6 : 3;

	// Draw the play button
	draw::enqueue(draw::rect{
		.rect = playButtonRect,
		.color = jaw::color::WHITE
	}, 1);
	draw::enqueue(draw::rect{
		.rect = jaw::recti(playButtonRect.tl + thickness, playButtonRect.br - thickness),
		.color = jaw::color::BLACK
	}, 2);
	draw::enqueue(draw::str{
		.rect = playButtonRect,
		.str = "\nPLAY",
		.color = jaw::color::WHITE,
		.font = hover ? buttonFontBold : buttonFont
	}, 3);

	// Check connected controllers
	// Find new controllers every 64 frames
	if (!(props->framecount & 63)) input::findNewGamepads();
	controller = input::getGamepad(0) != nullptr;
	draw::enqueue(draw::str{
		.rect = jaw::recti(0, props->size.y*9/10, props->size.x, props->size.y),
		.str = controller ? "Controller connected\nPress X to Play" : "No controller found\nUsing keyboard/mouse",
		.color = jaw::color::WHITE,
		.font = buttonFont
	}, 3);

	if (controller) {
		// Pressing X does the same thing as clicking play
		if (input::getGamepad(0)->sony.x.isDown) playButtonHandler(props);
	}
}

/*
*	GAME STATE
*/
constexpr jaw::argb emptyColor = jaw::color::DARK_GRAY;
constexpr jaw::argb snakeColor = jaw::color::GREEN;
constexpr jaw::argb appleColor = jaw::color::RED;

static jaw::fontid scoreFont = jaw::INVALID_ID;
static jaw::recti playArea;
static int16_t tileSize;
constexpr auto tiles = jaw::vec2i(25,25);

// An array of draw calls to draw each tile
// These can be quickly moved into the draw queue in one batch
static draw::drawCall tileDraws[tiles.x][tiles.y]{};

enum class move {
	up, left, down, right, none
};
static std::list<move> moveQueue;
static move dir;

static auto rng = std::mt19937((unsigned)time(NULL));
static auto distx = std::uniform_int_distribution(0, tiles.x-1);
static auto disty = std::uniform_int_distribution(0, tiles.y-1);
static std::list<jaw::vec2i> snake;
static jaw::vec2i apple;

static void randomizeApple() {
	// Place the apple somewhere the snake isn't
retry:
	apple = jaw::vec2i(distx(rng), disty(rng));
	for (auto &x : snake) {
		if (apple == x) goto retry;
	}

	tileDraws[apple.x][apple.y].rect.color = appleColor;
}

static void processMove(jaw::properties* props) {
	// Process the next move in the queue if there is one
	if (moveQueue.size() > 0) {
		move m = moveQueue.front();
		moveQueue.pop_front();

		switch (m) {
		case move::up:
			if (dir != move::down) dir = move::up;
			break;

		case move::left:
			if (dir != move::right) dir = move::left;
			break;

		case move::down:
			if (dir != move::up) dir = move::down;
			break;

		case move::right:
			if (dir != move::left) dir = move::right;
			break;

		case move::none:
			assert(false);
			[[fallthrough]];
		default:
			break;
		}
	}

	// Move the snake
	switch (dir) {
	case move::up:
		snake.push_front(snake.front() + jaw::vec2i(0, -1));
		tileDraws[snake.front().x][snake.front().y].rect.color = snakeColor;
		break;

	case move::left:
		snake.push_front(snake.front() + jaw::vec2i(-1, 0));
		tileDraws[snake.front().x][snake.front().y].rect.color = snakeColor;
		break;

	case move::down:
		snake.push_front(snake.front() + jaw::vec2i(0, 1));
		tileDraws[snake.front().x][snake.front().y].rect.color = snakeColor;
		break;

	case move::right:
		snake.push_front(snake.front() + jaw::vec2i(1, 0));
		tileDraws[snake.front().x][snake.front().y].rect.color = snakeColor;
		break;

	case move::none:
	default:
		break;
	}

	if (dir != move::none) {
		// Check for apple
		if (apple == snake.front()) {
			randomizeApple();
		}
		else {
			jaw::vec2i back = snake.back();
			snake.pop_back();
			if (back != snake.front()) tileDraws[back.x][back.y].rect.color = emptyColor;
		}

		// Check for out of bounds
		if (snake.front().x < 0 ||
			snake.front().x >= tiles.x ||
			snake.front().y < 0 ||
			snake.front().y >= tiles.y
		) {
			game_init(props);	// Reset the game
			return;
		}

		// Check for collision with self
		// It's impossible for the snake to collide with its first four segments
		if (snake.size() > 4) {
			auto it = snake.begin();
			std::advance(it, 4);	// Skip the first four snake segments
			for (; it != snake.end(); it++) {
				if (*it == snake.front()) {
					game_init(props);	// Reset the game
					return;
				}
			}
		}
	}

	// Continue processing moves while the game is running every 100ms regardless of framerate
	if (state::top() == gameState) {
		util::setTimer(props, jaw::millis(100), processMove);
	}
}

static void game_initOnce(jaw::properties *props) {
	// Calculate the playArea rectangle and tileSize from the tiles and screen size
	auto center = props->size/2;
	tileSize = (std::min(props->size.x, props->size.y) / std::max(tiles.x, tiles.y)) - 1;
	assert(tileSize >= 1);

	jaw::vec2i playAreaSize = tiles * tileSize;
	playArea = jaw::recti(center - playAreaSize/2, center + playAreaSize/2);

	// Set up draw calls for tiles
	for (int x = 0; x < tiles.x; x++) {
		for (int y = 0; y < tiles.y; y++) {
			tileDraws[x][y] = draw::make<draw::rect>(draw::rect{
				.rect = jaw::recti(
					playArea.tl.x + (x*tileSize) + 2,
					playArea.tl.y + (y*tileSize) + 2,
					playArea.tl.x + ((x+1)*tileSize) - 2,
					playArea.tl.y + ((y+1)*tileSize) - 2
				),
				.color = emptyColor
			}, 2);
		}
	}

	scoreFont = draw::newFont(draw::font{
		.name = "Arial",
		.size = 16
	});
}

static void game_init(jaw::properties *props) {
	draw::setBackgroundColor(jaw::color::DARK_BLUE);

	// Clear tile colors to empty
	for (int x = 0; x < tiles.x; x++) {
		for (int y = 0; y < tiles.y; y++) {
			tileDraws[x][y].rect.color = emptyColor;
		}
	}

	// Clear previous bindings and clickables
	input::clear();
	// Bind the escape key to return to the previous state, ie the menu
	input::bindKeyDown(key::ESC, [](jaw::properties*) { state::pop(); });

	moveQueue.clear();
	snake.clear();
	dir = move::none;
	snake.push_back(jaw::vec2i(tiles.x/2, tiles.y/2));
	tileDraws[tiles.x/2][tiles.y/2].rect.color = snakeColor;
	randomizeApple();

	processMove(props);
}

static void kbdInput() {
	if (input::getKey(key::W).isDown || input::getKey(key::UP).isDown) {
		moveQueue.push_back(move::up);
	}
	else if (input::getKey(key::A).isDown || input::getKey(key::LEFT).isDown) {
		moveQueue.push_back(move::left);
	}
	else if (input::getKey(key::S).isDown || input::getKey(key::DOWN).isDown) {
		moveQueue.push_back(move::down);
	}
	else if (input::getKey(key::D).isDown || input::getKey(key::RIGHT).isDown) {
		moveQueue.push_back(move::right);
	}
}

static void ctrlInput(const jaw::SonyGamepad &sony) {
	if (sony.up.isDown) {
		moveQueue.push_back(move::up);
	}
	else if (sony.left.isDown) {
		moveQueue.push_back(move::left);
	}
	else if (sony.down.isDown) {
		moveQueue.push_back(move::down);
	}
	else if (sony.right.isDown) {
		moveQueue.push_back(move::right);
	}

	// Return to the main menu if the start/options button is pressed
	if (sony.start.isDown) {
		state::pop();
	}
}

static void game_loop(jaw::properties *props) {
	// Process either keyboard or controller input
	// If the game started with a controller connected but it's now disconnected, 
	// use the keyboard but search for controllers to reconnect
	if (controller) {
		auto ctrl = input::getGamepad(0);
		if (ctrl) {
			// Assuming the controller is a sony (ie a DS4 or DualSense)
			// because that is the only controller type currently supported
			ctrlInput(ctrl->sony);
		}
		else {
			kbdInput();
			input::findNewGamepads();
		}
	}
	else {
		kbdInput();
	}

	// Draw black play area on top of the dark blue background
	draw::enqueue(draw::rect{
		.rect = playArea,
		.color = jaw::color::BLACK
	}, 0);

	// memcpy the contiguous array of tile draw calls into the draw queue
	draw::enqueueMany((draw::drawCall*)tileDraws, tiles.product());

	// Draw the score
	// util::tempalloc is a fast bump allocator that allocates from a fixed temporary buffer
	// The allocator is reset at the beginning of every frame
	// Do not store pointers returned by tempalloc across frame boundaries
	char *buf = util::tempalloc<char>(64);
	snprintf(buf, 64, "\n  Score: %zu", snake.size());
	draw::enqueue(draw::str{
		.rect = jaw::recti(0, props->size),
		.str = buf,
		.color = jaw::color::WHITE,
		.font = scoreFont
	}, 3);
}
