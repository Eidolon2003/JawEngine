add_executable(drawing drawing.cpp)
target_link_libraries(drawing PRIVATE jawengine)

if(MSVC)
    target_link_options(drawing PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(drawing PRIVATE "-Wl,-e,jaw_entry")
endif()