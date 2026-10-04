# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.12)
project("JawEngine")
set(CMAKE_CXX_STANDARD 20)

# Compilation Options
option(REQUIRE_AVX2 "Enable AVX2 optimizations" OFF)
option(NSPRITE "Disable sprite and animation APIs" OFF)
option(NSOUND "Disable sound API" OFF)
option(NINPUT "Disable input API" OFF)
option(NSTATE "Disable state API" OFF)

# Require clang
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "JawEngine requires Clang")
endif()

# Suppress clang's warning about out-of-order designated initializers
add_compile_options("-Wno-reorder-init-list")

# LTO
option(ENABLE_LTO "Enable Link-Time Optimization in Release builds" ON)
if(ENABLE_LTO)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	message("CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}")
	message("CMAKE_EXE_LINKER_FLAGS_RELEASE = ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	message("CMAKE_SHARED_LINKER_FLAGS_RELEASE = ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
	message("CMAKE_EXE_LINKER_FLAGS_DEBUG = ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	message("CMAKE_SHARED_LINKER_FLAGS_DEBUG = ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
endif()

# Include sub-projects.
add_subdirectory("jawengine")
add_subdirectory("TestApp")