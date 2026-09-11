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
 * This file path: kernel/mem/physical.c
 */
#include <mem/physical.h>

#include <mem/mem.h>
#include <panic.h>
#include <sync/spinlock.h>

uint64_t FREE_LIST_HEAD;
uint64_t TOTAL_FREE_PAGES;

static uint64_t align_up(uint64_t addr, uint64_t align) {
    return (addr + align - 1) & ~(align - 1);
}

static uint64_t align_dn(uint64_t addr, uint64_t align) {
    return addr & ~(align - 1);
}

void physical_init(void) {
    FREE_LIST_HEAD   = 0;
    TOTAL_FREE_PAGES = 0;

    for (uint64_t i = 0; i < MEMORY_INFO.region_count; i++) {
        memory_region_t *region = &MEMORY_INFO.regions[i];

        if (region->type != MEMORY_USABLE) continue;

        uint64_t start = align_up(region->base, PMM_PAGE_SIZE);
        uint64_t end   = \
            align_dn(region->base + region->length, PMM_PAGE_SIZE);

        for (uint64_t phys = start; phys < end; phys += PMM_PAGE_SIZE) {
            volatile uint64_t *next_ptr = \
                (uint64_t *)(phys + MEMORY_INFO.hhdm_offset);
            *next_ptr = FREE_LIST_HEAD;
            FREE_LIST_HEAD = phys;
            TOTAL_FREE_PAGES++;
        }
    }

    if (FREE_LIST_HEAD == 0) {
        panic("physical_init: No usable physical memory available (scanned %zu regions, total usable: %zu KB)",
              MEMORY_INFO.region_count,
              MEMORY_INFO.usable_memory / 1024);
    }
}

uint64_t pmm_get_usable(void) {
    return TOTAL_FREE_PAGES * PMM_PAGE_SIZE;
}
