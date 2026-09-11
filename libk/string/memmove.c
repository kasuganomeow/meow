/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* Copyright 2026 KasuganoMeow
   This file is part of the Meow.

   The Meow is free software: you can redistribute it and/or
   modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either
   version 3 of the License, or (at your option) any later version.

   The Meow is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public
   License along with the Meow. If not, see
   <https://www.gnu.org/licenses/>. */
/*
 * This file path: libk/src/string/memcpy.c
 */
#include <string.h>

#include <stdint.h>

void *memmove(void *s1, const void *s2, size_t n) {
    uint8_t* dst = (uint8_t*) s1;
	const uint8_t* src = (const uint8_t*) s2;
	if (dst < src) {
		for (size_t index = 0; index < n; index++)
			dst[index] = src[index];
	} else {
		for (size_t index = n; index != 0; index--)
			dst[index-1] = src[index-1];
	}
	return s1;
}
