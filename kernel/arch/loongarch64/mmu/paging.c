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
 * This file kernel/arch/loongarch64/mmu/paging.c
 */
#include <mmu/paging.h>

#include <limine.h>

#define LA_PTE_V      (1ULL << 0)
#define LA_PTE_D      (1ULL << 1)
#define LA_PTE_PLV_U  (3ULL << 2)
#define LA_PTE_MAT_CC (1ULL << 4)
#define LA_PTE_MAT_SU (2ULL << 4)
#define LA_PTE_W      (1ULL << 12)
#define LA_PTE_NX     (1ULL << 62)
#define LA_ADDR_MASK  0x0000FFFFFFFFF000ULL

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_paging_mode_request paging_mode_request = {
    .id       = LIMINE_PAGING_MODE_REQUEST_ID,
    .revision = 0,
    .mode     = LIMINE_PAGING_MODE_LOONGARCH_4LVL,
    .max_mode = LIMINE_PAGING_MODE_LOONGARCH_4LVL,
    .min_mode = LIMINE_PAGING_MODE_LOONGARCH_4LVL
};

uint64_t paging_make_table_entry(uint64_t phys_addr) {
    return (phys_addr & LA_ADDR_MASK) | LA_PTE_V;
}

uint64_t paging_make_leaf_entry(uint64_t phys_addr, uint64_t flags) {
    uint64_t pte = (phys_addr & LA_ADDR_MASK) | LA_PTE_V | LA_PTE_D;
    pte |= (flags & PAGE_UNCACHEABLE) ? LA_PTE_MAT_SU : LA_PTE_MAT_CC;
    if (flags     & PAGE_WRITABLE)      pte |= LA_PTE_W;
    if (flags     & PAGE_USER)          pte |= LA_PTE_PLV_U;
    if (!(flags   & PAGE_EXECUTABLE)) pte |= LA_PTE_NX;
    return pte;
}

uint64_t paging_pte_to_phys(uint64_t pte) {
    return pte & LA_ADDR_MASK;
}

bool paging_pte_is_present(uint64_t pte) {
    return (pte & LA_PTE_V) != 0;
}
