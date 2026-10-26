add_executable(basic-key-handling basic-key-handling.cpp)
target_link_libraries(basic-key-handling PRIVATE jawengine)

if(MSVC)
    target_link_options(TestApp PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(TestApp PRIVATE "-Wl,-e,jaw_entry")
endif()