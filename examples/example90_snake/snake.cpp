/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025-2026 Julian Williams
 *
 * JawEngine 0.2.2
 * https://github.com/Eidolon2003/JawEngine
 */

// Snake demo game!
// Typically you'd split this code into multiple files,
// but for this demo it's all here.

#include <ctime>
#include <deque>
#include <random>
#include <cstdio>
#include <cstdint>
#include <algorithm>

// This game doesn't use sound, so we can disable it
// If the engine lib is also compiled with -DNSOUND=ON,
// the sound API is completely removed, resulting in a smaller binary
#define JAW_NSOUND
#include <JawEngine.h>
#include <libs/ui.cpp>	// Include the implementation exactly once, or integrate it into your build

/*
*	FORWARD DECLARE STATE FUNCTIONS
*/
static void title_initOnce(jaw::properties*);	// A state's initOnce function is called once when it's created
static void title_init(jaw::properties*);		// A state's init function is called when it becomes active
static void title_deinit(jaw::properties*);		// A state's deinit function is called when it becomes inactive
static void title_loop(jaw::properties*);		// A state's loop function is called once every frame when active

static jaw::stateid gameState = jaw::INVALID_ID;
static void game_initOnce(jaw::properties*);
static void game_init(jaw::properties*);
static void game_deinit(jaw::properties*);
static void game_loop(jaw::properties*);

/*
*	MAIN
*/
int main() {
	jaw::properties props {
		.title = "Snake Demo",
		.size = jaw::vec2i(1280, 1024),
		.enableSubpixelTextRendering = true
	};

	// Start in the title screen
	engine::start(&props, { 
		.loop = title_loop,
		.init = title_init,
		.initOnce = title_initOnce,
		.deinit = title_deinit
	});
}

/*
*	TITLE SCREEN STATE
*/
static jaw::fontid titleFont = jaw::INVALID_ID;
static jaw::fontid buttonFont = jaw::INVALID_ID;
static bool controller = false;

// This is called once when the state is created
// Used for one-time initialization
static void title_initOnce(jaw::properties *props) {
	// Registering new fonts with the draw API
	titleFont = draw::newFont({
		.name = "Comic Sans MS",
		.size = (float)props->size.y/8,
		.bold = true,
		.alignx = draw::font::CENTERX
	});
	buttonFont = draw::newFont({
		.name = "Comic Sans MS",
		.size = (float)props->size.y/32,
		.alignx = draw::font::CENTERX,
		.aligny = draw::font::CENTERY
	});

	// Create the game state
	// This calls game_initOnce
	gameState = state::create(props, {
		.loop = game_loop,
		.init = game_init,
		.initOnce = game_initOnce,
		.deinit = game_deinit
	});
}

static void title_init(jaw::properties *props) {
	draw::setBackgroundColor(jaw::color::BLACK);

	input::bindKeyDown(key::ESC, [](jaw::properties*) { engine::stop(); });

	// Create the play button using the ui library
	auto playButtonRect = jaw::recti(
		props->size.x/2 - props->size.x/8 - 1,
		props->size.y/2 - props->size.y/16 - 1,
		props->size.x/2 + props->size.x/8,
		props->size.y/2 + props->size.y/16
	);
	(void)ui::createTextButton({
		.text = "PLAY",
		.font = buttonFont,
		.rect = playButtonRect,
		.select = [](ui::id, jaw::properties*) { state::push(gameState); }
	});
}

static void title_deinit(jaw::properties *props) {
	// Clean up the things we set up in title_init
	input::clear();
	ui::clear();
}

static void title_loop(jaw::properties *props) {
	// Draw the title text
	draw::enqueue<draw::str>({
		.rect = jaw::recti(jaw::vec2i(), props->size),
		.str = "JawEngine\nSnake",
		.color = jaw::color::WHITE,
		.font = titleFont
	}, 0);

	// Check connected controllers
	// Find new controllers every 64 frames
	if (!(props->framecount & 63)) input::findNewGamepads();
	controller = input::getGamepad(0) != nullptr;
	draw::enqueue<draw::str>({
		.rect = jaw::recti(0, props->size.y*9/10, props->size.x, props->size.y),
		.str = controller ? "Controller connected\nPress X to Play" : "No controller found\nUsing keyboard/mouse",
		.color = jaw::color::WHITE,
		.font = buttonFont
	}, 3);

	if (controller) {
		// Pressing X does the same thing as clicking play
		if (input::getGamepad(0)->sony.x.isDown) state::push(gameState);
	}
}

/*
*	GAME STATE
*/
constexpr jaw::argb emptyColor = jaw::color::DARK_GRAY;
constexpr jaw::argb snakeColor = jaw::color::GREEN;
constexpr jaw::argb appleColor = jaw::color::RED;

static jaw::fontid scoreFont = jaw::INVALID_ID;
static jaw::vec2i playAreaSize;
static jaw::recti playAreaRect;
static jaw::bmpid playAreaBmp;
static int16_t tileSize;
constexpr auto tiles = jaw::vec2i(25, 25);

enum class move {
	up, left, down, right, none
};
static std::deque<move> moveQueue;
static move dir;

static auto rng = std::mt19937((unsigned)time(NULL));
static auto distx = std::uniform_int_distribution(0, tiles.x-1);
static auto disty = std::uniform_int_distribution(0, tiles.y-1);
static std::deque<jaw::vec2i> snake;
static jaw::vec2i apple;

static void drawTile(jaw::vec2i tile, jaw::argb color) {
	draw::tobmp<draw::rect>({
		.rect = jaw::recti(
			tile*tileSize + 2,
			(tile+1)*tileSize - 2
		),
		.color = color
	}, playAreaBmp);
}

static void randomizeApple() {
	// Place the apple somewhere the snake isn't
retry:
	apple = jaw::vec2i(distx(rng), disty(rng));
	for (auto &x : snake) {
		if (apple == x) goto retry;
	}

	drawTile(apple, appleColor);
}

static void processMove(jaw::callbackid, jaw::properties* props) {
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
		drawTile(snake.front(), snakeColor);
		break;

	case move::left:
		snake.push_front(snake.front() + jaw::vec2i(-1, 0));
		drawTile(snake.front(), snakeColor);
		break;

	case move::down:
		snake.push_front(snake.front() + jaw::vec2i(0, 1));
		drawTile(snake.front(), snakeColor);
		break;

	case move::right:
		snake.push_front(snake.front() + jaw::vec2i(1, 0));
		drawTile(snake.front(), snakeColor);
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
			if (back != snake.front()) drawTile(back, emptyColor);
		}

		// Check for out of bounds
		if (snake.front().x < 0 ||
			snake.front().x >= tiles.x ||
			snake.front().y < 0 ||
			snake.front().y >= tiles.y)
		{
			state::pop();	// Get the state system to reset us
			state::push(gameState);
			return;
		}

		// Check for collision with self
		// It's impossible for the snake to collide with its first four segments
		if (snake.size() > 4) {
			auto it = snake.begin();
			std::advance(it, 4);	// Skip the first four snake segments
			for (; it != snake.end(); it++) {
				if (*it == snake.front()) {
					state::pop();	// Get the state system to reset us
					state::push(gameState);
					return;
				}
			}
		}
	}
}

static void game_initOnce(jaw::properties *props) {
	// Calculate the playArea rectangle and tileSize from the tiles and screen size
	auto center = props->size/2;
	tileSize = (std::min(props->size.x, props->size.y) / std::max(tiles.x, tiles.y)) - 1;
	assert(tileSize >= 1);

	playAreaSize = tiles * tileSize;
	playAreaRect = jaw::recti(center - playAreaSize/2, center + playAreaSize/2);

	// Create play area bitmap
	// A renderable bitmap can be drawn on directly by the draw API
	// This allows you to composite a complex image once, then redraw it cheaply
	playAreaBmp = draw::createRenderableBmp(playAreaSize);
	draw::tobmp<draw::rect>({
		.rect = jaw::recti(jaw::vec2i(), playAreaSize),
		.color = jaw::color::BLACK
	}, playAreaBmp);

	scoreFont = draw::newFont({
		.name = "Arial",
		.size = 16
	});
}

static void game_init(jaw::properties *props) {
	draw::setBackgroundColor(jaw::color::DARK_BLUE);

	// Clear tile colors to empty
	for (int x = 0; x < tiles.x; x++) {
		for (int y = 0; y < tiles.y; y++) {
			drawTile(jaw::vec2i(x, y), emptyColor);
		}
	}

	// Bind the escape key to return to the previous state, ie the menu
	input::bindKeyDown(key::ESC, [](jaw::properties*) { state::pop(); });

	// Call processMove every 100ms, regardless of framerate
	callback::create({
		.callback = processMove,
		.timeInterval = jaw::millis(100)
	});

	moveQueue.clear();
	snake.clear();
	dir = move::none;
	auto snakeStart = jaw::vec2i(tiles.x/2, tiles.y/2);
	snake.push_back(snakeStart);
	drawTile(snakeStart, snakeColor);
	randomizeApple();
}

static void game_deinit(jaw::properties *props) {
	// Clear what we set up in game_init
	input::clear();
	callback::clear();
}

static void kbdInput() {
	// isDown is only true for one frame when the key is pressed
	// isHeld is true continuously while the key is held down
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

	// Draw the play area bitmap
	draw::enqueue<draw::bmp>({
		.bmp = playAreaBmp,
		.src = jaw::recti(jaw::vec2i(), playAreaSize),
		.dest = playAreaRect
	}, 1);

	// Draw the score
	// util::frameAllocator is a fast bump allocator that allocates from a fixed temporary buffer
	// The allocator is reset at the beginning of every frame.
	// However, it's safe to draw a string from this memory using draw::str
	char *buf = (char*)util::frameAllocator->allocRaw(64);
	snprintf(buf, 64, "\n  Score: %zu", snake.size());
	draw::enqueue<draw::str>({
		.rect = jaw::recti(jaw::vec2i(), props->size),
		.str = buf,
		.color = jaw::color::WHITE,
		.font = scoreFont
	}, 3);
}