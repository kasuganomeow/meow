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
 * This file path: include/mem/mem.h
 */
#ifndef MEM_MEM_H
#define MEM_MEM_H 1

#include <stdint.h>

#define MAX_MEM_REGIONS 64

typedef enum {
    MEMORY_USABLE,
    MEMORY_RESERVED,
    MEMORY_ACPI_RECLAIMABLE,
    MEMORY_NVS,
    MEMORY_BADRAM,
    MEMORY_BOOTLOADER_RECLAIMABLE,
    MEMORY_KERNEL_AND_MODULES,
    MEMORY_FRAMEBUFFER,
    MEMORY_UNKNOWN
} memory_type_t;

typedef struct {
    uint64_t        base;
    uint64_t        length;
    memory_type_t   type;
} memory_region_t;

typedef struct {
    uint64_t        hhdm_offset;
    uint64_t        total_memory;
    uint64_t        usable_memory;
    uint64_t        total_phys_mem;
    uint64_t        max_phys_addr;
    uint64_t        region_count;
    memory_region_t regions[MAX_MEM_REGIONS];
} memory_info_t;

void mem_init(void);

extern memory_info_t MEMORY_INFO;

#endif
