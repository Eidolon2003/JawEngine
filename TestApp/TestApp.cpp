#include "../jawengine/JawEngine.h"

class MyApp : public jaw::AppInterface {
public:
	void Init() override {
		pSound->Loop("D:/Users/julia/Music/music.wav");
	}

	void Loop() override {
		if (pWindow->getLifetime() > std::chrono::seconds(2))
			pSound->Stop("D:/Users/julia/Music/music.wav");
	}

};

int main() {
	jaw::EngineProperties ep;
	jaw::AppProperties ap;
	jaw::StartEngine(new MyApp, ap, ep);
	return 0;
}