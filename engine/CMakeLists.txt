add_library (engine STATIC "engine.cpp" "engine.h" "JawEngine.h" "window.cpp" "window.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET engine PROPERTY CXX_STANDARD 20)
endif()