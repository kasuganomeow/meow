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
 * This file path: kernel/start.c
 */
#include <cpu/cpu.h>
#include <int/trap.h>
#include <limine.h>
#include <mmu/paging.h>
#include <panic.h>
#include <smp/smp.h>
#include <stdint.h>

extern void kmain(void);

[[gnu::used, gnu::section(".limine_requests_start")]]
static volatile uint64_t limine_requests_start_marker[] = \
    LIMINE_REQUESTS_START_MARKER;

[[gnu::used, gnu::section(".limine_requests")]]
static volatile uint64_t limine_base_revision[] =         \
    LIMINE_BASE_REVISION(6);

[[gnu::used, gnu::section(".limine_requests_end")]]
static volatile uint64_t limine_requests_end_marker[] =   \
    LIMINE_REQUESTS_END_MARKER;

void start(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        panic("1");
    }
    kmain();
}

void ap_main(struct limine_mp_info *info) {
    mmu_switch_pagemap(&KERNEL_PAGEMAP);

    trap_init_percpu();

    cpu_core_t *core = (cpu_core_t *)info->extra_argument;

    core->is_online = true;
    __atomic_fetch_add(&CPU_ONLINE_COUNT, 1, __ATOMIC_SEQ_CST);

    while (1) {
        spin();
    }
}
