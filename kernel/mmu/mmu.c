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
 * This file path: kernel/mmu/mmu.c
 */
#include <mmu/mmu.h>

#include <mem/mem.h>
#include <mem/xalloc.h>
#include <mmu/paging.h>
#include <panic.h>
#include <string.h>

size_t pt_index(uint64_t virt, int level) {
    return (virt >> (12 + 9 * level)) & 0x1FFULL;
}

uint64_t alloc_zeroed_table(void) {
    uint64_t paddr = palloc();
    if (paddr == 0) {
        panic("paging: Out of physical memory (failed to allocate frame for new page table directory)");
    }

    void *vaddr = (void *)(paddr + MEMORY_INFO.hhdm_offset);
    memset(vaddr, 0, PAGE_SIZE);
    return paddr;
}

uint64_t mmu_get_phys(pagemap_t *map, uint64_t virt) {
    uint64_t current_table_paddr = map->top_level_paddr;

    for (int level = 3; level >= 1; level--) {
        uint64_t *table_vaddr = \
            (uint64_t *)(current_table_paddr + MEMORY_INFO.hhdm_offset);
        size_t idx = pt_index(virt, level);
        uint64_t entry = table_vaddr[idx];

        if (!paging_pte_is_present(entry)) {
            return 0;
        }
        current_table_paddr = paging_pte_to_phys(entry);
    }

    uint64_t *l0_table = \
        (uint64_t *)(current_table_paddr + MEMORY_INFO.hhdm_offset);
    uint64_t leaf = l0_table[pt_index(virt, 0)];
    if (!paging_pte_is_present(leaf)) {
        return 0;
    }

    return paging_pte_to_phys(leaf) | (virt & (PAGE_SIZE - 1));
}
