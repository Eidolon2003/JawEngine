add_executable (TestApp "TestApp.cpp" )
target_link_libraries(TestApp engine)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TestApp PROPERTY CXX_STANDARD 20)
endif()