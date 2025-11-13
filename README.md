# JawEngine 0.2.0
JawEngine is a game engine for building 2D games in C++. It is designed to be minimal, explicit, and simple, while still providing the core features needed to create games. See the Snake demo example at `TestApp/TestApp.cpp`

Version 0.2.0 is a complete rewrite of 0.1.0; old code will not work! This project is a work-in-progress. For now, only Windows is supported via DirectX (Direct2D, XAudio, and DirectInput), but cross-platform support is planned. For now, programs built with JawEngine run on WINE with minimal issues.

Important: Only Sony controllers are currently supported; tested working on DS4 and DualSense controllers

## Features
### JawEngine is divided into several APIs that are designed to be as decoupled as possible. Each API has its own namespace

Current APIs include: 
- `state` - Maintains a stack-based state system
- `draw` - 2D rendering; immediate mode rendering with retained resources
- `sound` - 44.1K, 16b, stereo WAV playback
- `asset` - Loads and parses asset files (bmp, wav, and ini)
- `input` - Handles keyboard, mouse, and controller input; supports automated bindings and callbacks
- `sprite` - Supports automatically moving, drawing, and self-destructing sprites
- `anim` - Supports animations for sprites
- `util` - A collection of useful tools, such as `tempalloc` or `setTimer`

## Hello, world!
```
#include <JawEngine.h>

static void loop(jaw::properties *p) {
    draw::enqueue(draw::str{
        .rect = jaw::recti({0,0}, p->size),
        .str = "Hello, world!",
        .color = jaw::color::WHITE
        }, 0);
}

int main() {
    jaw::properties p;
    p.size = { 200,150 };
    p.scale = 4;
    engine::start(&p, nullptr, nullptr, loop);
}
```

## Compilation on Windows
1) Download and install MSVC Build Tools for the "Desktop Development with C++" workload: https://visualstudio.microsoft.com/visual-cpp-build-tools
    - Note: If you have Visual Studio installed, you should be able to skip this step
2) Install Git: `winget install Microsoft.Git`
3) Clone the repository
4) Open the "x64 Native Tools Command Prompt" and navigate to the project's root directory
5) Run `cmake --preset native-release` then `cmake --build --preset native-release`
6) Run `out\build\native-release\TestApp\TestApp.exe` to run the demo

## Cross-compilation on Linux
1) Ensure that you have `git`, `cmake`, and the `x86_64-w64-mingw32-g++` compiler
2) Compile as with Windows, but with `cmake --preset windows-release` and `cmake --build --preset windows-release`
3) Run with `wine64`

## Tips
The engine is centered around a stack-based state system. It's important to note that nothing is automatically cleaned up when a state is popped. You should call `input::clear()` in your states' init functions to clear all old bindings (if any) before setting up that state's specific bindings. The same goes for `sprite::clear()`, `util::clearTimers`, and so on.

Resources created by the `asset`, `sound`, and `draw` APIs live for the entire runtime of the engine and are automatically cleaned up at the end. There is no need (or indeed ability) to manually free them.

## License
JawEngine 0.2.0 is licensed under the Mozilla Public License 2.0 (MPL 2.0)

This license encourages the open development of JawEngine while also allowing developers to use it freely, including in closed source projects. 