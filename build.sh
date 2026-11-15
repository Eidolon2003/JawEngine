#!/bin/bash

MODE="debug"
PLATFORM="windows"
EXAMPLES=""
CLEAN="false"
BUILD="true"

for arg; do
	if [[ "$arg" = "release" ]]; then
		MODE="release"
	elif [[ "$arg" = "debug" ]]; then
		MODE="debug"
	elif [[ "$arg" = "profile" ]]; then
		MODE="profile"
	elif [[ "$arg" = "examples" ]]; then
		EXAMPLES="-DCOMPILE_EXAMPLES=ON"
	elif [[ "$arg" = "clean" ]]; then
		CLEAN="true"
	elif [[ "$arg" = "nobuild" ]]; then
		BUILD="false"
	else
		echo "Unknown argument: $arg"
		exit 1
	fi
done

if [[ "$CLEAN" = "true" ]]; then
	rm -rf out
else
	rm ./out/TestApp.exe &> /dev/null
fi

if [[ "$BUILD" = "true" ]]; then
	cmake --preset "$PLATFORM"-"$MODE" $EXAMPLES
	cmake --build --preset "$PLATFORM"-"$MODE"
	cp ./out/build/"$PLATFORM"-"$MODE"/TestApp/TestApp.exe ./out/TestApp.exe &> /dev/null
fi
