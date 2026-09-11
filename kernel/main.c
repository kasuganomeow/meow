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
 * This file path: kernel/main.c
 */
#include <con/con.h>
#include <con/font.h>
#include <con/kprint.h>
#include <int/trap.h>
#include <mem/mem.h>
#include <mem/physical.h>
#include <mem/virtual.h>
#include <mmu/paging.h>
#include <panic.h>
#include <smp/smp.h>

void kmain(void) {
    kprint("Hello, World!\n");

    /* con */
    font_init();
    con_init();
    kprint_flush();

    /* int */
    trap_init();

    /* mem */
    mem_init();
    physical_init();

    /* mmu */
    paging_init();

    /* mem */
    virtual_init();

    /* smp */
    smp_init();

    kprint("It's Kernel Space!\n");

    panic("No Found /sbin/init");
}
