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
 * This file path: libk/include/stddef.h
 */
#ifndef STDDEF_H
#define STDDEF_H 1

#define NULL           ((void *)0)
#define unreachable()  __builtin_unreachable()
#define offsetof(P, D) ((size_t)&(((P*)0)->D))

#include <stdint.h>

typedef int64_t  ptrdiff_t;
typedef void *   nullptr_t;
typedef int32_t  wchar_t;
typedef uint64_t size_t;

#endif
