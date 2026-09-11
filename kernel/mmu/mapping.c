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
 * This file path: kernel/mmu/mapping.c
 */
#include <mmu/mapping.h>

#include <mem/mem.h>
#include <mmu/paging.h>
#include <sync/spinlock.h>
#include <string.h>

static spinlock_t MAPPING_LOCK = SPINLOCK_INIT;

bool mapping_page
(
    pagemap_t *map,
    uint64_t virt,
    uint64_t phys,
    uint64_t flags
) {
    spinlock_acquire(&MAPPING_LOCK);

    uint64_t current_table_paddr = map->top_level_paddr;

    for (int level = 3; level >= 1; level--) {
        uint64_t *table_vaddr = \
            (uint64_t *)(current_table_paddr + MEMORY_INFO.hhdm_offset);
        size_t idx = pt_index(virt, level);
        uint64_t entry = table_vaddr[idx];

        if (!paging_pte_is_present(entry)) {
            uint64_t new_table = alloc_zeroed_table();
            table_vaddr[idx] = paging_make_table_entry(new_table);
            current_table_paddr = new_table;
        } else {
            current_table_paddr = paging_pte_to_phys(entry);
        }
    }

    uint64_t *l0_table = \
        (uint64_t *)(current_table_paddr + MEMORY_INFO.hhdm_offset);
    size_t idx = pt_index(virt, 0);
    l0_table[idx] = paging_make_leaf_entry(phys, flags);

    mmu_flush_cache(virt);

    spinlock_release(&MAPPING_LOCK);

    return true;
}

bool mapping_range
(
    pagemap_t *map,
    uint64_t virt,
    uint64_t phys,
    uint64_t size,
    uint64_t flags
) {
    uint64_t aligned_virt = virt & ~(PAGE_SIZE - 1);
    uint64_t aligned_phys = phys & ~(PAGE_SIZE - 1);
    uint64_t aligned_size = \
        (
            (size + (virt & (PAGE_SIZE - 1)) + PAGE_SIZE - 1) \
            & ~(PAGE_SIZE - 1)
        );

    for
    (
        uint64_t offset = 0;
        offset < aligned_size;
        offset += PAGE_SIZE
    ) {
        if (!mapping_page
            (
                map, aligned_virt + offset,
                aligned_phys + offset,
                flags
            )) {
            return false;
        }
    }
    return true;
}

void unmapping_page(pagemap_t *map, uint64_t virt) {
    spinlock_acquire(&MAPPING_LOCK);

    uint64_t current_table_paddr = map->top_level_paddr;

    for (int level = 3; level >= 1; level--) {
        uint64_t *table_vaddr = \
            (uint64_t *)(current_table_paddr + MEMORY_INFO.hhdm_offset);
        size_t idx = pt_index(virt, level);
        uint64_t entry = table_vaddr[idx];

        if (!paging_pte_is_present(entry)) {
            return;
        }
        current_table_paddr = paging_pte_to_phys(entry);
    }

    uint64_t *l0_table = \
        (uint64_t *)(current_table_paddr + MEMORY_INFO.hhdm_offset);
    l0_table[pt_index(virt, 0)] = 0;
    mmu_flush_cache(virt);

    spinlock_release(&MAPPING_LOCK);
}
