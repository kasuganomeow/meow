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
 * This file path: arch/x86_64/cpu/cpu.c
 */
#include <cpu/cpu.h>

void spin() {
    asm volatile("pause" ::: "memory");
}

void stop() {
    asm volatile(
        "cli\n\t"
        "1:\n\t"
        "    hlt\n\t"
        "    jmp 1b\n\t"
        ::: "memory"
    );
    __builtin_unreachable();
}
