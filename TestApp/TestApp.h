#pragma once
#include <iostream>

#include "../engine/JawEngine.h"

class MyApp : public jaw::Application {
public:
	void Loop() override;
};