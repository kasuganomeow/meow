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
 * This file path: libk/include/stdint.h
 */
#ifndef STDINT_H
#define STDINT_H 1

typedef signed char	  int8_t;
typedef short		     int16_t;
typedef int			     int32_t;
typedef long		     int64_t;

typedef int8_t		     int_fast8_t;
typedef int64_t		  int_fast16_t;
typedef int64_t		  int_fast32_t;
typedef int64_t		  int_fast64_t;

typedef int8_t		     int_least8_t;
typedef int16_t		  int_least16_t;
typedef int32_t		  int_least32_t;
typedef int64_t		  int_least64_t;

typedef int64_t		  intmax_t;
typedef int64_t		  intptr_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

typedef uint8_t		  uint_fast8_t;
typedef uint64_t	     uint_fast16_t;
typedef uint64_t	     uint_fast32_t;
typedef uint64_t	     uint_fast64_t;

typedef uint8_t		  uint_least8_t;
typedef uint16_t	     uint_least16_t;
typedef uint32_t	     uint_least32_t;
typedef uint64_t	     uint_least64_t;

typedef uint64_t	     uintmax_t;
typedef uint64_t	     uintptr_t;

#endif
