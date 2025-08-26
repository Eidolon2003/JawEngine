add_executable(TestApp "TestApp.cpp")
target_link_libraries(TestApp jawengine)
set_property(TARGET TestApp PROPERTY CXX_STANDARD 20)