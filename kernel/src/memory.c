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
 * This file path: kernel/src/memory.c
 */
#include <memory.h>
#include <limine.h>
#include <panic.h>
#include <stdint.h>

extern struct limine_memmap_request memmap_request;
extern struct limine_hhdm_request   hhdm_request;

memory_info_t MEMORY_INFO = {0};

void memory_info_init(void) {
    struct limine_memmap_response *memmap = \
        (struct limine_memmap_response *)memmap_request.response;
    struct limine_hhdm_response *hhdm = \
        (struct limine_hhdm_response *)hhdm_request.response;
    if (!memmap || !hhdm) {
        panic();
    }

    MEMORY_INFO.offset  = hhdm->offset;

    MEMORY_INFO.region_count = memmap->entry_count;
    if (MEMORY_INFO.region_count > 64)
        MEMORY_INFO.region_count = 64;

    for (uint64_t i = 0; i < MEMORY_INFO.region_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];

        MEMORY_INFO.regions[i].base   = entry->base;
        MEMORY_INFO.regions[i].length = entry->length;
        MEMORY_INFO.regions[i].usable = \
            (entry->type == LIMINE_MEMMAP_USABLE);

        MEMORY_INFO.total += entry->length;

        if (MEMORY_INFO.regions[i].usable) {
            MEMORY_INFO.usable += entry->length;
        } else {
            MEMORY_INFO.reserved += entry->length;
        }

        uint64_t end = entry->base + entry->length;
        if (end > MEMORY_INFO.highest_address) {
            MEMORY_INFO.highest_address = end;
        }
    }
}
