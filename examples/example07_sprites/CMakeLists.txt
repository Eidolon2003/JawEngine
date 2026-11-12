add_executable(sprites sprites.cpp)
target_link_libraries(sprites PRIVATE jawengine)

if(MSVC)
    target_link_options(sprites PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(sprites PRIVATE "-Wl,-e,jaw_entry")
endif()