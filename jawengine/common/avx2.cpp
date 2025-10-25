// This file contains AVX2 optimized code whether or not REQUIRE_AVX2 is on.

#include "avx2.h"
#include <intrin.h>

bool isAVX2() {
	int flags[4];
	// Calling __cpuid with 0 gets the highest valid ID
	// We need ID of at least 7 to support AVX2
	__cpuid(flags, 0);

	if (flags[0] >= 7) {
		// Check for AVX2 support from the processor
		__cpuid(flags, 7);
		if (flags[1] & (1 << 5)) {
			// Check for support from the operating system.
			// A native Windows machine will pass the ==6 test
			auto xgetbv = _xgetbv(0);
			return (xgetbv & 6) == 6;
		}
		else return false;
	}
	else return false;
}

static void multiplyAlpha_scalar(jaw::argb *dst, const jaw::argb *src, size_t n) {
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

void multiplyAlpha_avx2(jaw::argb *dst, const jaw::argb *src, size_t n) {
	size_t i = 0;
	size_t rounded_n = n & ~7;

	for (; i < rounded_n; i += 8) {
		// Load 8x 32bit pixels
		__m256i px = _mm256_loadu_si256((__m256i*)(src + i));

		// Unpack the upper and lower 16 bytes into sets of 16-bit values
		__m256i lo = _mm256_cvtepu8_epi16(_mm256_castsi256_si128(px));
		__m256i hi = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(px, 1));

		// Get just the alpha value in all bytes
		static const __m256i alpha_shuffle_mask = _mm256_set_epi8(
			15, 15, 15, 15,
			11, 11, 11, 11,
			7, 7, 7, 7,
			3, 3, 3, 3,
			15, 15, 15, 15,
			11, 11, 11, 11,
			7, 7, 7, 7,
			3, 3, 3, 3
		);
		__m256i alpha = _mm256_shuffle_epi8(px, alpha_shuffle_mask);

		// Unpack the alpha into 2 16-bit halves just like we did with the colors
		__m256i alpha_lo = _mm256_cvtepu8_epi16(_mm256_castsi256_si128(alpha));
		__m256i alpha_hi = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(alpha, 1));

		// Multiply colors by alpha
		__m256i result_lo = _mm256_mullo_epi16(lo, alpha_lo);
		__m256i result_hi = _mm256_mullo_epi16(hi, alpha_hi);

		// Add 128
		static const __m256i broadcast128 = _mm256_set1_epi16(128);
		result_lo = _mm256_add_epi16(result_lo, broadcast128);
		result_hi = _mm256_add_epi16(result_hi, broadcast128);

		// shift right 8 (dividing by 256 instead of 255, slightly inaccurate but fast)
		result_lo = _mm256_srli_epi16(result_lo, 8);
		result_hi = _mm256_srli_epi16(result_hi, 8);

		// Pack the lo and hi halves into one again
		__m256i result = _mm256_packus_epi16(result_lo, result_hi);
		result = _mm256_permute4x64_epi64(result, 0xD8);

		// Restore original alpha values
		static const __m256i alpha_bytes_mask = _mm256_set_epi8(
			(char)128, (char)0, (char)0, (char)0, (char)128, (char)0, (char)0, (char)0,
			(char)128, (char)0, (char)0, (char)0, (char)128, (char)0, (char)0, (char)0,
			(char)128, (char)0, (char)0, (char)0, (char)128, (char)0, (char)0, (char)0,
			(char)128, (char)0, (char)0, (char)0, (char)128, (char)0, (char)0, (char)0
		);
		result = _mm256_blendv_epi8(result, px, alpha_bytes_mask);

		// Store result
		_mm256_storeu_si256((__m256i*)(dst + i), result);
	}

	if (rounded_n < n) {
		multiplyAlpha_scalar(dst + i, src + i, n - rounded_n);
	}
}