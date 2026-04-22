#pragma once

namespace engine {
	void start(jaw::properties*, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop);
}