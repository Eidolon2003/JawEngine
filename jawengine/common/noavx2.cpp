#include "../JawEngine.h"
#include "avx2.h"

#if (defined _WIN32 || defined _WIN64)
#include <windows.h>
#endif 

#include <intrin.h>

jaw::Sysinfo jaw::sysinfo;

static inline void cpuid(int regs[4], int leaf, int subleaf = 0) {
#if defined(_MSC_VER)
	__cpuidex(regs, leaf, subleaf);
#elif defined(__GNUC__) || defined(__clang__)
	unsigned int eax, ebx, ecx, edx;
	__asm__ volatile("cpuid"
		: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
		: "a"(leaf), "c"(subleaf));
	regs[0] = eax; regs[1] = ebx; regs[2] = ecx; regs[3] = edx;
#else
	regs[0] = regs[1] = regs[2] = regs[3] = 0;
#endif
}

static inline uint64_t xgetbv(uint32_t index) {
#if defined(_MSC_VER)
	return _xgetbv(index);
#elif defined(__GNUC__) || defined(__clang__)
	uint32_t eax, edx;
	__asm__ volatile(".byte 0x0f,0x01,0xd0" : "=a"(eax), "=d"(edx) : "c"(index));
	return ((uint64_t)edx << 32) | eax;
#else
	return 0;
#endif
}

static bool isAVX2() {
	int flags[4];
	// Calling __cpuid with 0 gets the highest valid ID
	// We need ID of at least 7 to support AVX2
	cpuid(flags, 0);

	if (flags[0] >= 7) {
		// Check for AVX2 support from the processor
		cpuid(flags, 7);
		if (flags[1] & (1 << 5)) {
			// Check for support from the operating system.
			// A native Windows machine will pass the ==6 test
			auto x = xgetbv(0);
			return (x & 6) == 6;
		}
		else return false;
	}
	else return false;
}

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
extern "C" void __cdecl mainCRTStartup();
extern "C" void __cdecl jaw_entry() {
	jaw::sysinfo.avx2 = false;
	jaw::sysinfo.wine = isWINE();
	if (jaw::sysinfo.wine) {
		// This assumes that any system running WINE supports AVX2
		// This will NOT always be the case, and will crash on systems that don't
		// WINE does not correctly support the CPUID flag and xgetbv checks that I do for Windows below
		jaw::sysinfo.avx2 = true;
	}
	else {
		jaw::sysinfo.avx2 = isAVX2();
	}

#ifdef JAW_REQUIRE_AVX2
	if (!jaw::sysinfo.avx2) {
#if (defined _WIN32 || defined _WIN64)
		MessageBox(
			NULL,
			"This application requires the AVX2 instruction set.\nYour CPU does not support AVX2.",
			"Instruction Set Not Supported",
			MB_OK | MB_ICONWARNING
		);
		exit(1);
#endif
	}
#endif
	mainCRTStartup();
}

void multiplyAlpha_scalar(jaw::argb *dst, const jaw::argb *src, size_t n) {
	const uint8_t *src_bytes = (const uint8_t*)src;
	uint8_t *dst_bytes = (uint8_t*)dst;
	for (size_t i = 0; i < n * 4; i += 4) {
		// ideally we'd round(x / 255)
		// (x + 128) >> 8 is pretty close and a lot faster
		dst_bytes[i+0] = (src_bytes[i+0] * src_bytes[i+3] + 128) >> 8;
		dst_bytes[i+1] = (src_bytes[i+1] * src_bytes[i+3] + 128) >> 8;
		dst_bytes[i+2] = (src_bytes[i+2] * src_bytes[i+3] + 128) >> 8;
		dst_bytes[i+3] = src_bytes[i+3];
	}
}
