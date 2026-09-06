/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* Copyright 2026 KasuganoMeow
   This file is part of the MEOW.

   The MEOW is free software: you can redistribute it and/or
   modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either
   version 3 of the License, or (at your option) any later version.

   The MEOW is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public
   License along with the MEOW. If not, see
   <https://www.gnu.org/licenses/>. */
/*
 * This file path: kernel/arch/x86_64/include/gdt.h
 */
#ifndef GDT_H
#define GDT_H 1

#include <stdint.h>

#define GDT_LIMIT_1_OFFSET     0
#define GDT_BASE_1_OFFSET      2
#define GDT_BASE_2_OFFSET      4
#define GDT_ACCESS_OFFSET      5
#define GDT_GRANULARITY_OFFSET 6
#define GDT_BASE_3_OFFSET      7

#define GDT_TABLE_SIZE         8

#define GDTR_LIMIT_OFFSET      0
#define GDTR_BASE_OFFSET       2

extern void gdt_load(void);

void gdt_set_descriptor(
    uint8_t table, uint16_t limit_1,
    uint16_t base_1, uint8_t base_2,
    uint8_t access, uint8_t granularity,
    uint8_t base_3
);
void gdt_init(void);

#endif
