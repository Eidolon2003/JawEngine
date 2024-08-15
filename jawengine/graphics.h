#pragma once

#include "JawEngine.h"

namespace jaw {

	class InternalGraphicsInterface : public GraphicsInterface {
	public:
		virtual ~InternalGraphicsInterface() {}
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};


	inline constexpr int16_t ScaleUp(int16_t x, float scale) { return (int16_t)(x * scale); }
	inline constexpr int16_t ScaleDown(int16_t x, float scale) { return (int16_t)(x / scale); }
};