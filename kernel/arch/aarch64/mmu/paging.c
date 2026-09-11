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
 * This file path: kernel/arch/aarch64/mmu/paging.c
 */
#include <mmu/paging.h>

#include <limine.h>

#define ARM_PTE_VALID        (1ULL << 0)
#define ARM_PTE_TYPE_TABLE   (1ULL << 1)
#define ARM_PTE_TYPE_PAGE    (1ULL << 1)
#define ARM_PTE_USER         (1ULL << 6)
#define ARM_PTE_RO           (1ULL << 7)
#define ARM_PTE_SH_INNER     (3ULL << 8)
#define ARM_PTE_AF           (1ULL << 10)
#define ARM_PTE_PXN          (1ULL << 53)
#define ARM_PTE_UXN          (1ULL << 54)
#define ARM_ADDR_MASK        0x0000FFFFFFFFF000ULL

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_paging_mode_request paging_mode_request = {
    .id       = LIMINE_PAGING_MODE_REQUEST_ID,
    .revision = 0,
    .mode     = LIMINE_PAGING_MODE_AARCH64_4LVL,
    .max_mode = LIMINE_PAGING_MODE_AARCH64_4LVL,
    .min_mode = LIMINE_PAGING_MODE_AARCH64_4LVL
};

uint64_t paging_make_table_entry(uint64_t phys_addr) {
    return (phys_addr & ARM_ADDR_MASK) | ARM_PTE_VALID | ARM_PTE_TYPE_TABLE;
}

uint64_t paging_make_leaf_entry(uint64_t phys_addr, uint64_t flags) {
    uint64_t pte = \
        (phys_addr & ARM_ADDR_MASK) |       \
        ARM_PTE_VALID | ARM_PTE_TYPE_PAGE | \
        ARM_PTE_AF |                        \
        ARM_PTE_SH_INNER;
    if (!(flags & PAGE_WRITABLE))   pte |= ARM_PTE_RO;
    if (flags   & PAGE_USER)          pte |= ARM_PTE_USER;
    if (!(flags & PAGE_EXECUTABLE)) pte |= (ARM_PTE_PXN | ARM_PTE_UXN);
    return pte;
}

uint64_t paging_pte_to_phys(uint64_t pte) {
    return pte & ARM_ADDR_MASK;
}

bool paging_pte_is_present(uint64_t pte) {
    return (pte & ARM_PTE_VALID) != 0;
}
