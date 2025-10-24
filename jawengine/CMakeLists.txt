set(JAWENGINE_SOURCES)

if(WIN32)
	message("JawEngine for Windows")
	list(APPEND JAWENGINE_SOURCES
		"windows/win32_asset.cpp"
		"windows/win32_engine.cpp"
		"windows/win32_win.cpp"
		"windows/win32_d2d.cpp"
	)

	if(NOT NSOUND)
		list(APPEND JAWENGINE_SOURCES
			"windows/win32_xaudio.cpp"
		)
	endif()
endif()

if(NOT NSPRITE)
	list(APPEND JAWENGINE_SOURCES
		"common/sprite.cpp"
	)
endif()

if(NOT NINPUT)
	list(APPEND JAWENGINE_SOURCES
		"common/input.cpp"
	)
endif()

if(NOT NSTATE)
	list(APPEND JAWENGINE_SOURCES
		"common/state.cpp"
	)
endif()

add_library(jawengine STATIC ${JAWENGINE_SOURCES})
set_property(TARGET jawengine PROPERTY CXX_STANDARD 20)

if(WIN32)
	target_link_libraries(jawengine
		winmm.lib
		d2d1.lib
		windowscodecs.lib
		dwrite.lib
	)
endif()

if(REQUIRE_AVX2)
	message("JawEngine REQUIRE_AVX2 ON")
	if(MSVC)
		target_compile_options(jawengine PRIVATE /arch:AVX2)
	else()
		target_compile_options(jawengine PRIVATE -mavx2)
	endif()
endif()

if(NSPRITE)
	message("JawEngine NSPRITE ON")
	if(MSVC)
		target_compile_options(jawengine PRIVATE /DJAW_NSPRITE)
	else()
		target_compile_options(jawengine PRIVATE -DJAW_NSPRITE)
	endif()
endif()

if(NSOUND)
	message("JawEngine NSOUND ON")
	if(MSVC)
		target_compile_options(jawengine PRIVATE /DJAW_NSOUND)
	else()
		target_compile_options(jawengine PRIVATE -DJAW_NSOUND)
	endif()
endif()

if(NINPUT)
	message("JawEngine NINPUT ON")
	if(MSVC)
		target_compile_options(jawengine PRIVATE /DJAW_NINPUT)
	else()
		target_compile_options(jawengine PRIVATE -DJAW_NINPUT)
	endif()
endif()

if(NSTATE)
	message("JawEngine NSTATE ON")
	if(MSVC)
		target_compile_options(jawengine PRIVATE /DJAW_NSTATE)
	else()
		target_compile_options(jawengine PRIVATE -DJAW_NSTATE)
	endif()
endif()