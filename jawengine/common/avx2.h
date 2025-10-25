#pragma once
#include "../types.h"

bool isAVX2();
void multiplyAlpha_avx2(jaw::argb *dst, const jaw::argb *src, size_t n);