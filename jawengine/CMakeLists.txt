add_library(mylib STATIC 
	"engine.cpp" 
	"engine.h"
	"JawEngine.h"
	"window.cpp"
	"window.h"
	"input.cpp"
	"input.h"
	"graphics.h"
	"d2d.h"
	"d2d.cpp"
	"dsound.h"
	"dsound.cpp"
 "sprite.cpp")
set_property(TARGET mylib PROPERTY CXX_STANDARD 20)

if(WIN32)
	target_link_libraries(mylib
		winmm.lib
		d2d1.lib
		windowscodecs.lib
		dwrite.lib
		dsound.lib
		dxguid.lib
	)

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
endif()