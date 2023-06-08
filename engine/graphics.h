#pragma once

#include "JawEngine.h"

namespace jaw {

	class InternalGraphicsInterface : public GraphicsInterface {
	public:
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void setSize(uint16_t x, uint16_t y) = 0;
	};

};