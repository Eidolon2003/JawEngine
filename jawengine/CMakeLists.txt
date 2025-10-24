set(JAWENGINE_SOURCES
	"JawEngine.h"
	"winvkc.h"
	"types.h"
	"asset.h"
	"draw.h"
	"input.h"
	"state.h"
	"sound.h"
	"common/internal_asset.h"
	"common/internal_input.h"
	"common/input.cpp"
	"common/internal_state.h"
	"common/state.cpp"
	"common/internal_sound.h"
)

if(WIN32)
	message("JawEngine for Windows")
	list(APPEND JAWENGINE_SOURCES
		"windows/win32_asset.cpp"
		"windows/win32_engine.cpp"
		"windows/win32_internal_win.h"
		"windows/win32_win.cpp"
		"windows/win32_internal_draw.h"
		"windows/win32_d2d.cpp"
		"windows/win32_xaudio.cpp"
	)
endif()

if(NOT NSPRITE)
	list(APPEND JAWENGINE_SOURCES
		"sprite.h"
		"common/internal_sprite.h"
		"common/sprite.cpp"
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
		target_compile_options(jawengine PRIVATE /DNSPRITE)
	else()
		target_compile_options(jawengine PRIVATE -DNSPRITE)
	endif()
endif()