# JawEngine 0.2.1 - (0.2.2 in progress)
JawEngine is a game engine for building 2D games in C++. It is designed to be minimal, explicit, and simple, while still providing the core features needed to create games. For usage examples, check out the examples directory!

This project is a work-in-progress. For now, only Windows is supported via DirectX (Direct2D, XAudio, and DirectInput), but cross-platform support is planned. For now, programs built with JawEngine run on WINE with minimal issues.

Important: Only Sony controllers are currently supported; tested working on DS4 and DualSense controllers

## Features
JawEngine is divided into several APIs that are designed to be as decoupled as possible, ie they do not depend on each other. Each API has its own namespace.

Current APIs include: 
- `state` - Maintains a stack-based state system and memory allocator
- `draw` - 2D rendering; immediate mode rendering with retained resources
- `sound` - 44.1K, 16b, stereo audio playback
- `asset` - Loads and parses asset files (bmp, wav, ini, and abc synth)
- `input` - Handles keyboard, mouse, and controller input; supports automated bindings and callbacks
- `callback` - Allows registering callbacks with the engine which will be called at a user-defined interval
- `util` - A collection of useful tools, such as `frameAllocator` or `setTimer`

## Compilation on Windows
1) Download and install MSVC Build Tools for the "Desktop Development with C++" workload: https://visualstudio.microsoft.com/visual-cpp-build-tools 
    - Be sure to also select clang as an optional add on. The project will not compile with MSVC
    - Note: If you have Visual Studio installed, you should be able to skip this step
2) Install Git: `winget install Microsoft.Git`
3) Clone the repository
4) Open the "x64 Native Tools Command Prompt" and navigate to the project's root directory
5) Run `cmake --preset native-release` then `cmake --build --preset native-release` to build the library
6) Use `cmake --preset native-release -DCOMPILE_EXAMPLES=ON` to compile all of the examples in the examples directory

## Cross-compilation on Linux (WIP)
1) Ensure that you have `git`, `cmake`, and the `clang` compiler with Windows libraries
2) Compile as with Windows, but with `cmake --preset windows-release` and `cmake --build --preset windows-release`
3) Run with `wine64`

## Architectural Notes
The core of the engine, which is comprised of the APIs listed above, is intended to be the minimum possible toolkit of engine managed resources required. Higher order structures, such as UI and Sprites, are provided as libraries that compose the engine's core primitives. For exmaple, see `jawengine/libs/ui.h`

The engine is centered around a stack-based state system. Each state has four associated functions: `initOnce`, `init`, `deinit`, and `loop`:
- `initOnce` is called one time when the state is created
- `init` is called when the state is entered
- `deinit` is called when the state is exited
- `loop` is called each frame when the state is active

The engine acknowledges four basic lifetimes for data: application lifetime, engine lifetime, state lifetime, and frame lifetime:
- Application lifetime data is managed by the game. The engine will not create nor destroy it automatically, so it will survive engine restarts. This is useful when restarting the engine to change settings for example.
- Engine lifetime data is created at engine startup and destroyed at engine shutdown. A typical example of this would be assets, such as bitmaps. A state's `initOnce` is its opportunity to load any engine lifetime data it requires
- State lifetime data is created at state entry and destroyed at state exit. Examples of this include keybindings, callbacks, and data allocated by state::allocator. A state's `init` and `deinit` are its opportunity to manage state lifetime data
- Frame lifetime data is valid only for the current frame. `util::frameAllocator` is automatically cleared at the start of the frame, and can be used for any scratch memory that's needed during frame computations

The engine makes extensive use of `util::slotAllocator` for managing engine objects such as callbacks and clickables. The slot allocator is a solution to the problem of referring to engine managed resources with raw pointers, which is unsafe. An ID returned by a slotAllocator is guaranteed to stably refer to a particular piece of engine managed memory. When that object is destroyed, attempting to lookup its ID will yield a nullptr. `util::arenaAllocator` is a simple bump allocator which frees its data all at once. The engine provides two managed arenas itself, `util::frameAllocator` for frame lifetime data, and `state::allocator` for state lifetime data. Both of these allocators are available for general use, if the application requires them.

## License
JawEngine 0.2.1 is licensed under the Mozilla Public License 2.0 (MPL 2.0)

This license encourages the open development of JawEngine while also allowing developers to use it freely, including in closed source projects. 
