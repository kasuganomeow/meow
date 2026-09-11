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
 * This file path: arch/x86_64/smp/smp.c
 */
#include <smp/smp.h>

#include <cpu/cpu.h>
#include <limine.h>
#include <mem/kalloc.h>
#include <mem/xalloc.h>
#include <panic.h>
#include <stdint.h>

extern void ap_main(struct limine_mp_info *info);

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_mp_request mp_request = {
    .id       = LIMINE_MP_REQUEST_ID,
    .revision = 0,
    .flags    = LIMINE_MP_REQUEST_X86_64_X2APIC
};

cpu_core_t     *CPUS             = NULL;
size_t          CPU_TOTAL_COUNT  = 1;
volatile size_t CPU_ONLINE_COUNT = 1;

void smp_init(void) {
    struct limine_mp_response *resp = mp_request.response;
    if (resp == NULL || resp->cpu_count <= 1) {
        CPU_TOTAL_COUNT  = 1;
        CPU_ONLINE_COUNT = 1;
        CPUS = (cpu_core_t *)kalloc(sizeof(cpu_core_t));
        if (!CPUS) panic("SMP: Failed to allocate CPU core structure");
        CPUS[0].cpu_id       = 0;
        CPUS[0].kernel_stack = NULL;
        CPUS[0].is_online    = true;
        return;
    }

    CPU_TOTAL_COUNT = resp->cpu_count;
    CPUS = (cpu_core_t *)kalloc(sizeof(cpu_core_t) * CPU_TOTAL_COUNT);
    if (!CPUS) {
        panic("SMP: Failed to allocate CPU array for %zu cores", CPU_TOTAL_COUNT);
    }

    CPUS[0].cpu_id       = 0;
    CPUS[0].kernel_stack = NULL;
    CPUS[0].is_online    = true;

    size_t ap_idx = 1;

    for (size_t index = 0; index < CPU_TOTAL_COUNT; index++) {
        struct limine_mp_info *cpu_info = resp->cpus[index];

        if (cpu_info->lapic_id == resp->bsp_lapic_id) {
            continue;
        }

        CPUS[ap_idx].cpu_id       = ap_idx;
        CPUS[ap_idx].is_online    = false;
        CPUS[ap_idx].kernel_stack = valloc(4);
        if (!CPUS[ap_idx].kernel_stack) {
            panic("SMP: Failed to allocate stack for CPU %zu (requested %d pages)", ap_idx, 4);
        }

        cpu_info->extra_argument = (uint64_t)&CPUS[ap_idx];
        __atomic_store_n(&cpu_info->goto_address, ap_main, __ATOMIC_RELEASE);

        ap_idx++;
    }

    while (
        __atomic_load_n(&CPU_ONLINE_COUNT, __ATOMIC_ACQUIRE) <
        CPU_TOTAL_COUNT
    ) {
        spin();
    }
}
