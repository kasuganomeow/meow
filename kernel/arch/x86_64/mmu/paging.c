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
 * This file path: kernel/arch/x86_64/mmu/paging.c
 */
#include <mmu/paging.h>

#include <limine.h>
#include <mmu/mmu.h>

#define X86_PTE_P     (1ULL << 0)
#define X86_PTE_RW    (1ULL << 1)
#define X86_PTE_US    (1ULL << 2)
#define X86_PTE_PWT   (1ULL << 3)
#define X86_PTE_PCD   (1ULL << 4)
#define X86_PTE_NX    (1ULL << 63)
#define X86_ADDR_MASK 0x000FFFFFFFFFF000ULL

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_paging_mode_request paging_mode_request = {
    .id       = LIMINE_PAGING_MODE_REQUEST_ID,
    .revision = 0,
    .mode     = LIMINE_PAGING_MODE_X86_64_4LVL,
    .max_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
    .min_mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

uint64_t paging_make_table_entry(uint64_t phys_addr) {
    return (phys_addr & X86_ADDR_MASK) | X86_PTE_P | X86_PTE_RW | X86_PTE_US;
}

uint64_t paging_make_leaf_entry(uint64_t phys_addr, uint64_t flags) {
    uint64_t pte = (phys_addr & X86_ADDR_MASK) | X86_PTE_P;
    if (flags   & PAGE_WRITABLE)      pte |= X86_PTE_RW;
    if (flags   & PAGE_USER)          pte |= X86_PTE_US;
    if (!(flags & PAGE_EXECUTABLE)) pte |= X86_PTE_NX;
    if (flags   & PAGE_UNCACHEABLE)   pte |= (X86_PTE_PCD | X86_PTE_PWT);
    return pte;
}

uint64_t paging_pte_to_phys(uint64_t pte) {
    return pte & X86_ADDR_MASK;
}

bool paging_pte_is_present(uint64_t pte) {
    return (pte & X86_PTE_P) != 0;
}
