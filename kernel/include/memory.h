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
 * This file path: kernel/include/memory.h
 */
#ifndef MEMORY_H
#define MEMORY_H 1

#include <stdint.h>

typedef struct {
    uint64_t        base;
    uint64_t        length;
    uint8_t         usable;
} memory_region_t;

typedef struct {
    uint64_t        total;
    uint64_t        usable;
    uint64_t        reserved;
    uint64_t        highest_address;
    uint64_t        offset;
    uint64_t        region_count;
    memory_region_t regions[64];
} memory_info_t;

void memory_info_init(void);

#endif
