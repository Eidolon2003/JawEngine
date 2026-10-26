add_executable(basic-mouse-handling basic-mouse-handling.cpp)
target_link_libraries(basic-mouse-handling PRIVATE jawengine)

if(MSVC)
    target_link_options(TestApp PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(TestApp PRIVATE "-Wl,-e,jaw_entry")
endif()