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

void *memcpy(void *restrict s1, const void *restrict s2, size_t n) {
    unsigned char *dest = (unsigned char *)s1;
	const unsigned char *srcptr = (const unsigned char *)s2;
	for (uint64_t i = 0; i < n; i++)
		dest[i] = srcptr[i];
	return s1;
}
