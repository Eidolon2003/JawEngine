add_library(engine STATIC "engine.cpp" "engine.h" "JawEngine.h" "window.cpp" "window.h" "input.cpp" "input.h"  "graphics.h" "d2d.h" "d2d.cpp")

if(WIN32)
	target_link_libraries(engine winmm.lib)
	target_link_libraries(engine d2d1.lib)
	target_link_libraries(engine dwrite.lib)
	target_link_libraries(engine dxguid.lib)
	target_link_libraries(engine windowscodecs.lib)
	target_link_libraries(engine dsound.lib)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET engine PROPERTY CXX_STANDARD 20)
endif()