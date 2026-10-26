add_executable(basic-gamepad-handling basic-gamepad-handling.cpp)
target_link_libraries(basic-gamepad-handling PRIVATE jawengine)

if(MSVC)
    target_link_options(TestApp PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(TestApp PRIVATE "-Wl,-e,jaw_entry")
endif()