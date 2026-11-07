add_executable(drawing drawing.cpp)
target_link_libraries(drawing PRIVATE jawengine)

if(MSVC)
    target_link_options(TestApp PRIVATE "/ENTRY:jaw_entry")
else()
    target_link_options(TestApp PRIVATE "-Wl,-e,jaw_entry")
endif()