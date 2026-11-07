@echo off

set "MODE=debug"
set "PLATFORM=native"
set "EXAMPLES="
set "CLEAN=false"
set "BUILD=true"

:PARSE
if "%~1"=="" (
	goto EXECUTE
)
if /i "%~1"=="release" (
	shift
	set "MODE=release"
	goto PARSE
)
if /i "%~1"=="debug" (
	shift
	set "MODE=debug"
	goto PARSE
)
if /i "%~1"=="profile" (
	shift
	set "MODE=profile"
	goto PARSE
)
if /i "%~1"=="examples" (
	shift
	set "EXAMPLES=-DCOMPILE_EXAMPLES=ON"
	goto PARSE
)
if /i "%~1"=="clean" (
	shift
	set "CLEAN=true"
	goto PARSE
)
if /i "%~1"=="nobuild" (
	shift
	set "BUILD=false"
	goto PARSE
)
echo Unknown argument: %~1
exit /b 1

:EXECUTE
if "%CLEAN%"=="true" (
	rmdir /s /q out
) else (
	del .\out\TestApp.exe >nul 2>&1
)
if "%BUILD%"=="true" (
	cmake --preset %PLATFORM%-%MODE% %EXAMPLES%
	cmake --build --preset %PLATFORM%-%MODE%
	copy .\out\build\%PLATFORM%-%MODE%\TestApp\TestApp.exe .\out\TestApp.exe >nul 2>&1
)