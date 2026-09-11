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
 * This file path: kernel/mmu/paging.c
 */
#include <mmu/paging.h>

#include <limine.h>
#include <mem/mem.h>
#include <mmu/mapping.h>
#include <panic.h>

extern uint8_t __kernel_start[];
extern uint8_t __kernel_end[];

pagemap_t KERNEL_PAGEMAP;

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_executable_address_request \
    exec_addr_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID,
    .revision = 0
};

void paging_init(void) {
    if (exec_addr_request.response == NULL) {
        panic("paging_init: Bootloader failed to provide executable address (cannot determine kernel physical/virtual load base)");
    }

    KERNEL_PAGEMAP.top_level_paddr = alloc_zeroed_table();

    for (uint64_t i = 0; i < MEMORY_INFO.region_count; i++) {
        memory_region_t *region = &MEMORY_INFO.regions[i];

        uint64_t flags = PAGE_READABLE | PAGE_WRITABLE;
        mapping_range
        (
            &KERNEL_PAGEMAP,
            region->base + MEMORY_INFO.hhdm_offset,
            region->base,
            region->length,
            flags
        );
    }

    if (exec_addr_request.response != NULL) {
        uint64_t kphys = exec_addr_request.response->physical_base;
        uint64_t kvirt = exec_addr_request.response->virtual_base;

        uint64_t ksize = (uint64_t)__kernel_end - (uint64_t)__kernel_start;

        mapping_range(
            &KERNEL_PAGEMAP, kvirt, kphys, ksize,
            PAGE_READABLE | PAGE_WRITABLE | PAGE_EXECUTABLE
        );
    }

    mmu_switch_pagemap(&KERNEL_PAGEMAP);
}
