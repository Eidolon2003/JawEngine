add_library(jawengine STATIC 
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
	"win32_asset.cpp" 
	"state.cpp"
	"state.h"
	"internal_state.h"
)
	
set_property(TARGET jawengine PROPERTY CXX_STANDARD 20)

if(WIN32)
	target_link_libraries(jawengine
		winmm.lib
		d2d1.lib
		windowscodecs.lib
		dwrite.lib
		#dxguid.lib
	)
endif()