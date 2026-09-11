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
 * This file path: kernel/panic.c
 */
#include <panic.h>

#include <con/kprint.h>
#include <cpu/cpu.h>
#include <mem/mem.h>

[[gnu::noreturn]]
void panic_impl(const char *file, int line, const char *func, const char *fmt, ...) {
    kprint("\nKERNEL PANIC!\n");
    kprint("PANICKED AT: (MEOW)/%s:%d in %s()\n", file, line, func);
    kprint("REASON: ");

    va_list args;
    va_start(args, fmt);
    kvprint(fmt, args);
    va_end(args);

    kprint("\nSYSTEM HALTED\n");

    while (1) {
        stop();
    }
}
