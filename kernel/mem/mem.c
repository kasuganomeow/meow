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
 * This file path: kernel/mem/mem.c
 */
#include <mem/mem.h>

#include <limine.h>
#include <panic.h>
#include <stdint.h>

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

memory_info_t MEMORY_INFO;

static memory_type_t convert_limine_type(uint64_t limine_type) {
    switch (limine_type) {
        case LIMINE_MEMMAP_USABLE:
            return MEMORY_USABLE;
        case LIMINE_MEMMAP_RESERVED:
            return MEMORY_RESERVED;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return MEMORY_ACPI_RECLAIMABLE;
        case LIMINE_MEMMAP_ACPI_NVS:
            return MEMORY_NVS;
        case LIMINE_MEMMAP_BAD_MEMORY:
            return MEMORY_BADRAM;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return MEMORY_BOOTLOADER_RECLAIMABLE;
        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
            return MEMORY_KERNEL_AND_MODULES;
        case LIMINE_MEMMAP_FRAMEBUFFER:
            return MEMORY_FRAMEBUFFER;
        default:
            return MEMORY_UNKNOWN;
    }
}

void mem_init(void) {
    if (!hhdm_request.response || !memmap_request.response)
        panic("mem_init: Bootloader failed to provide essential memory information (HHDM: %s, Memmap: %s)",
              hhdm_request.response ? "OK" : "MISSING",
              memmap_request.response ? "OK" : "MISSING");

    struct limine_hhdm_response *hhdm_response     = \
        hhdm_request.response;
    struct limine_memmap_response *memmap_response = \
        memmap_request.response;
    struct limine_memmap_entry **memmap_entries    = \
        memmap_response->entries;

    MEMORY_INFO.hhdm_offset    = hhdm_response->offset;
    MEMORY_INFO.total_memory   = 0;
    MEMORY_INFO.total_phys_mem = 0;
    MEMORY_INFO.usable_memory  = 0;
    MEMORY_INFO.max_phys_addr  = 0;

    if (memmap_response->entry_count > MAX_MEM_REGIONS) {
        MEMORY_INFO.region_count = MAX_MEM_REGIONS;
    } else {
        MEMORY_INFO.region_count = memmap_response->entry_count;
    }

    for (uint64_t index = 0; index < MEMORY_INFO.region_count; index++) {
        struct limine_memmap_entry *memmap_entry = memmap_entries[index];

        MEMORY_INFO.regions[index].base   = memmap_entry->base;
        MEMORY_INFO.regions[index].length = memmap_entry->length;
        MEMORY_INFO.regions[index].type   = \
            convert_limine_type(memmap_entry->type);

        MEMORY_INFO.total_memory += memmap_entry->length;

        uint64_t end = memmap_entry->base + memmap_entry->length;

        if (MEMORY_INFO.regions[index].type == MEMORY_USABLE) {
            MEMORY_INFO.usable_memory += memmap_entry->length;
            if (end > MEMORY_INFO.total_phys_mem)
                MEMORY_INFO.total_phys_mem = end;
        }

        if (
            (memmap_entry->base + memmap_entry->length) > \
            MEMORY_INFO.max_phys_addr
        ) {
            MEMORY_INFO.max_phys_addr = \
                (memmap_entry->base + memmap_entry->length);
        }
    }
}
