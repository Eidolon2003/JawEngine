add_executable(mouse-handling mouse-handling.cpp)
target_link_libraries(mouse-handling PRIVATE jawengine)

if(MSVC)
    target_link_options(mouse-handling PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(mouse-handling PRIVATE "-Wl,-e,jaw_entry")
endif()