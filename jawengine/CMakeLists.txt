add_library(mylib STATIC 
	"JawEngine.h"
	"winvkc.h"
	"types.h"
	"win32_internal_win.h"
	"win32_win.cpp"
	"win32_engine.cpp"
	"draw.h"
	"win32_internal_draw.h"
	"win32_d2d.cpp"
	"input.h"
	"internal_input.h"
	"input.cpp"
	"asset.h"
	"internal_asset.h"
	"win32_asset.cpp" "state.cpp" "state.h"  "internal_state.h")
set_property(TARGET mylib PROPERTY CXX_STANDARD 20)

if(WIN32)
	target_link_libraries(mylib
		winmm.lib
		d2d1.lib
		windowscodecs.lib
		dwrite.lib
		#dxguid.lib
	)
	#[[]
	find_library(WINMM "winmm.lib")
	find_library(D2D1 "d2d1.lib")
	find_library(WINDOWSCODECS "windowscodecs.lib")
	find_library(DWRITE "dwrite.lib")
	find_library(DSOUND "dsound.lib")
	find_library(DXGUID "dxguid.lib")

	add_custom_command(
		TARGET mylib POST_BUILD
		COMMAND lib.exe /OUT:${CMAKE_CURRENT_BINARY_DIR}/jawengine.lib 
			$<TARGET_FILE:mylib>
			${WINMM}
			${D2D1}
			${WINDOWSCODECS}
			${DWRITE}
			${DSOUND}
			${DXGUID}
	)
	]]
endif()