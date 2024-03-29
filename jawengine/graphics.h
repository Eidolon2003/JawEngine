#pragma once

#include "JawEngine.h"

namespace jaw {

	class InternalGraphicsInterface : public GraphicsInterface {
	public:
		virtual ~InternalGraphicsInterface() {}
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void setSize(uint16_t x, uint16_t y) = 0;
	};


	inline constexpr uint16_t ScaleUp(uint16_t x, float scale) { return (uint16_t)(x * scale); }
	inline constexpr uint16_t ScaleDown(uint16_t x, float scale) { return (uint16_t)(x / scale); }
};