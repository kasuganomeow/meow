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
 * This file path: kernel/include/smp/smp.h
 */
#ifndef SMP_H
#define SMP_H 1

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t       cpu_id;
    void          *kernel_stack;
    volatile bool  is_online;
} cpu_core_t;

extern cpu_core_t     *CPUS;
extern size_t          CPU_TOTAL_COUNT;
extern volatile size_t CPU_ONLINE_COUNT;

void smp_init(void);

#endif
