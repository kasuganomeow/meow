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
 * This file path: kernel/arch/x86_64/src/gdt.c
 */
#include <gdt.h>

extern uint8_t gdt_table[];
extern uint8_t gdtr[];

void gdt_set_descriptor(
    uint8_t table, uint16_t limit_1,
    uint16_t base_1, uint8_t base_2,
    uint8_t access, uint8_t granularity,
    uint8_t base_3
) {
    uint8_t offset = GDT_TABLE_SIZE * table;
    *(uint16_t *)(gdt_table + GDT_LIMIT_1_OFFSET     + offset) = limit_1;
    *(uint16_t *)(gdt_table + GDT_BASE_1_OFFSET      + offset) = base_1;
    *(uint8_t  *)(gdt_table + GDT_BASE_2_OFFSET      + offset) = base_2;
    *(uint8_t  *)(gdt_table + GDT_ACCESS_OFFSET      + offset) = access;
    *(uint8_t  *)(gdt_table + GDT_GRANULARITY_OFFSET + offset) = granularity;
    *(uint8_t  *)(gdt_table + GDT_BASE_3_OFFSET      + offset) = base_3;
}

void gdt_init(void) {
    gdt_set_descriptor(0, 0, 0, 0, 0, 0, 0);
    gdt_set_descriptor(1, 0, 0, 0, 0x9A, (0x20 & 0xF0), 0);
    gdt_set_descriptor(2, 0, 0, 0, 0x92, (0x00 & 0xF0), 0);

    *(uint16_t *)(gdtr + GDTR_LIMIT_OFFSET) = 23;
    *(uint64_t *)(gdtr + GDTR_BASE_OFFSET) = (uint64_t)gdt_table;

    gdt_load();
}
