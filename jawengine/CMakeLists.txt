add_library(jawengine STATIC 
	"JawEngine.h"
	"winvkc.h"
	"types.h"
	"asset.h"
	"draw.h"
	"input.h"
	"state.h"
	"sprite.h"
	"sound.h"
	"common/internal_asset.h"
	"common/internal_input.h"
	"common/input.cpp"
	"common/internal_state.h"
	"common/state.cpp"
	"common/internal_sprite.h"
	"common/sprite.cpp"
	"common/internal_sound.h"
	"windows/win32_asset.cpp"
	"windows/win32_engine.cpp"
	"windows/win32_internal_win.h"
	"windows/win32_win.cpp"
	"windows/win32_internal_draw.h"
	"windows/win32_d2d.cpp"
	"windows/win32_xaudio.cpp")
	
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