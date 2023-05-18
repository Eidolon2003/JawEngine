add_library (engine "engine.cpp" "engine.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TestApp PROPERTY CXX_STANDARD 20)
endif()