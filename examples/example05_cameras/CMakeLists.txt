add_executable(camera camera.cpp)
target_link_libraries(camera PRIVATE jawengine)

if(MSVC)
    target_link_options(camera PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(camera PRIVATE "-Wl,-e,jaw_entry")
endif()