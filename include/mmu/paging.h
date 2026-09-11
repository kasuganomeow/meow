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
 * This file path: include/mmu/paging.h
 */
#ifndef MMU_PAGING_H
#define MMU_PAGING_H 1

#define PAGE_SIZE        4096ULL
#define PAGE_SHIFT       12

#define PAGE_READABLE    (1ULL << 0)
#define PAGE_WRITABLE    (1ULL << 1)
#define PAGE_EXECUTABLE  (1ULL << 2)
#define PAGE_USER        (1ULL << 3)
#define PAGE_UNCACHEABLE (1ULL << 4)

#include <mmu/mmu.h>
#include <stdint.h>

void     paging_init(void);
uint64_t paging_make_table_entry(uint64_t phys_addr);
uint64_t paging_make_leaf_entry(uint64_t phys_addr, uint64_t flags);
uint64_t paging_pte_to_phys(uint64_t pte);
bool     paging_pte_is_present(uint64_t pte);

extern pagemap_t KERNEL_PAGEMAP;

#endif
