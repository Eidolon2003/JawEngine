add_executable(hello-world hello-world.cpp)
target_link_libraries(hello-world PRIVATE jawengine)

if(MSVC)
    target_link_options(hello-world PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(hello-world PRIVATE "-Wl,-e,jaw_entry")
endif()