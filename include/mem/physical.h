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
 * This file path: kernel/include/memory/physical.h
 */
#ifndef MEM_PHYSICAL_H
#define MEM_PHYSICAL_H 1

#define PMM_PAGE_SIZE 4096

#include <stdint.h>

extern uint64_t FREE_LIST_HEAD;
extern uint64_t TOTAL_FREE_PAGES;

void     physical_init(void);
uint64_t pmm_get_usable(void);

#endif
