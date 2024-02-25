# JawEngine
JawEngine is a simple 2D game engine written in C++. Features include:
- Animated sprites
- Multiple drawing layers with transparency
- WAV audio
- Event-driven input

This project is a work-in-progress. For now, only Windows is supported via DirectX, but cross-platform support is planned. Programs built using this engine work under Wine in my experience. Check out [my snake clone](https://github.com/Eidolon2003/Snake) as an example

## Setup
For Windows development, I use [Microsoft Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/). This project requires the "Desktop development with C++" and "Game development with C++" packages. If you already have Visual Studio installed but don't have these packages, they can be added via `Tools > Get Tools and Features...`.

Once Visual Studio is installed, click "Clone a repository" at the startup menu and enter the url `https://github.com/Eidolon2003/JawEngine.git`. With the repo cloned, Visual Studio will be able to run the CMake script and build the project. The resulting file `jawengine.lib` can be found at `...\JawEngine\out\build\x64-release\jawengine\jawengine.lib`. Substitue "x64-release" for "x64-debug" if you compiled in debug mode. The resulting .lib files are not identical, and the correct file is required for compiling your game in either release or debug mode.

At this point, you can either edit "TestApp.cpp", or create your own project and link the library. All Windows library dependencies are bundled into jawengine.lib, so no other links are necessary.

## Hello World
```
#include <JawEngine.h>

class HelloWorld : public jaw::AppInterface {
public:
    void Init() override {
        pGraphics->DrawString(L"Hello, world!", jaw::Rect(0, 0, 200, 20), 0);
    }

    void Loop() override {}
};

int main() {
    jaw::EngineProperties ep;
    jaw::AppProperties ap;
    jaw::StartEngine(new HelloWorld, ap, ep);
}
```
