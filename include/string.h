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
 * This file path: libk/include/string.h
 */
#ifndef LIBK_STRING_H
#define LIBK_STRING_H 1

#include <stddef.h>

void   *memcpy(void *restrict s1, const void *restrict s2, size_t n);
void   *memmove(void *s1, const void *s2, size_t n);
void   *memset(void *s, int c, size_t n);
size_t  strlen(const char *s);

#endif
