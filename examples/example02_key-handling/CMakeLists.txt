add_executable(key-handling key-handling.cpp)
target_link_libraries(key-handling PRIVATE jawengine)

if(MSVC)
    target_link_options(key-handling PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(key-handling PRIVATE "-Wl,-e,jaw_entry")
endif()