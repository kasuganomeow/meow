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
 * This file path: kernel/arch/x86_64/src/gdt.S
 */
.extern gdtr

.section .bss
.align 8
.global gdt_table
gdt_table:
    .rept 3
    .word 0x0000
    .word 0x0000
    .byte 0x00
    .byte 0x00
    .byte 0x00
    .byte 0x00
    .endr

.global gdtr
gdtr:
    .word 0x0000
    .quad 0x000000

.section .text
.global gdt_load
gdt_load:
    cli

    lgdt gdtr

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    pushq $0x08
    pushq $.L1
    lretq
.L1:
    sti
    retq
