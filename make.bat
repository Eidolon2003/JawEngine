@echo off
rmdir /s /q out
cmake --preset native-debug
cmake --build --preset native-debug
copy .\out\build\native-debug\TestApp\TestApp.exe .\out\TestApp.exe >nul 2>&1