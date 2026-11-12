add_executable(snake snake.cpp)
target_link_libraries(snake PRIVATE jawengine)

if(MSVC)
    target_link_options(snake PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(snake PRIVATE "-Wl,-e,jaw_entry")
endif()