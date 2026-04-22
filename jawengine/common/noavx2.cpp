#include "../JawEngine.h"
#include "internal_engine.h"
#include "avx2.h"	// isAVX2

#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif 

static bool isWINE() {
#if (defined _WIN32 || defined _WIN64)
	static bool cached = false;
	static bool wine = false;

	if (!cached) {
		HMODULE ntdll = GetModuleHandleA("ntdll.dll");
		wine = ntdll && GetProcAddress(ntdll, "wine_get_version");
		cached = true;
	}
	return wine;
#else
	return false;
#endif
}

#include <iostream>
// TODO: update this routine to probe for AVX2 support by trying to execute an instruction
void jaw::start(jaw::properties* props, jaw::statefn initOnce, jaw::statefn init, jaw::statefn loop) {
	if (props == nullptr) return;

	// Check CPU feature support
	bool avx2 = false;
	if (isWINE()) {
		// This assumes that any system running WINE supports AVX2
		// This will NOT always be the case, and will crash on systems that don't
		// WINE does not correctly support the CPUID flag and xgetbv checks that I do for Windows below
		avx2 = true;
		std::cerr << "Warning: Assuming AVX2 support under WINE. This may crash on CPUs without AVX2 extensions\n";
	}
	else {
		avx2 = isAVX2();
	}
#ifdef __AVX2__
	if (avx2) {
		props->cpuid.avx2 = true;
		return;
	}
	else {
#if (defined _WIN32 || defined _WIN64)
		MessageBox(NULL, "Your CPU does not support AVX2", "Instruction Set Not Supported", MB_OK | MB_ICONWARNING);
#endif
		exit(1);
	}
#else
	props->cpuid.avx2 = avx2;
#endif

	engine::start(props, initOnce, init, loop);
}