add_executable(gamepad-handling gamepad-handling.cpp)
target_link_libraries(gamepad-handling PRIVATE jawengine)

if(MSVC)
    target_link_options(gamepad-handling PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(gamepad-handling PRIVATE "-Wl,-e,jaw_entry")
endif()