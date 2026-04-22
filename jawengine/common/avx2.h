/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025 Julian Williams
 *
 * JawEngine 0.2.0
 * https://github.com/Eidolon2003/JawEngine
 */

#pragma once
#include "../types.h"

void multiplyAlpha_avx2(jaw::argb *dst, const jaw::argb *src, size_t n);
void multiplyAlpha_scalar(jaw::argb *dst, const jaw::argb *src, size_t n);