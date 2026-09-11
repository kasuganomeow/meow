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
 * This file path: kernel/arch/riscv64/mmu/paging.c
 */
#include <mmu/paging.h>

#include <limine.h>

#define RISCV_PTE_V (1ULL << 0)
#define RISCV_PTE_R (1ULL << 1)
#define RISCV_PTE_W (1ULL << 2)
#define RISCV_PTE_X (1ULL << 3)
#define RISCV_PTE_U (1ULL << 4)
#define RISCV_PTE_A (1ULL << 6)
#define RISCV_PTE_D (1ULL << 7)

#define SATP_MODE_SV48 (9ULL << 60)

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_paging_mode_request paging_mode_request = {
    .id       = LIMINE_PAGING_MODE_REQUEST_ID,
    .revision = 0,
    .mode     = LIMINE_PAGING_MODE_RISCV_SV48,
    .max_mode = LIMINE_PAGING_MODE_RISCV_SV48,
    .min_mode = LIMINE_PAGING_MODE_RISCV_SV48
};

uint64_t paging_make_table_entry(uint64_t phys_addr) {
    return ((phys_addr >> 12) << 10) | RISCV_PTE_V;
}

uint64_t paging_make_leaf_entry(uint64_t phys_addr, uint64_t flags) {
    uint64_t pte = \
        ((phys_addr >> 12) << 10) | RISCV_PTE_V | RISCV_PTE_A | RISCV_PTE_D;
    if (flags & PAGE_READABLE)   pte |= RISCV_PTE_R;
    if (flags & PAGE_WRITABLE)   pte |= RISCV_PTE_W;
    if (flags & PAGE_EXECUTABLE) pte |= RISCV_PTE_X;
    if (flags & PAGE_USER)       pte |= RISCV_PTE_U;
    return pte;
}

uint64_t paging_pte_to_phys(uint64_t pte) {
    return ((pte >> 10) & 0x003FFFFFFFFFFFULL) << 12;
}

bool paging_pte_is_present(uint64_t pte) {
    return (pte & RISCV_PTE_V) != 0;
}
