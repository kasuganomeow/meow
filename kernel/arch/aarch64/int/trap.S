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
 * This file path: kernel/arch/aarch64/int/trap.S
 */
.section .text

.extern trap_handler

.macro vector_entry type
    .balign 128
    sub sp, sp, #288
    stp x0, x1, [sp, #0]
    mov x0, #\type
    str x0, [sp, #248]
    b   trap_handler_entry
.endm

.global trap_vector_table
.balign 2048
trap_vector_table:
    vector_entry 0
    vector_entry 1
    vector_entry 2
    vector_entry 3

    vector_entry 4
    vector_entry 5
    vector_entry 6
    vector_entry 7

    vector_entry 8
    vector_entry 9
    vector_entry 10
    vector_entry 11

    vector_entry 12
    vector_entry 13
    vector_entry 14
    vector_entry 15

.global trap_handler_entry
trap_handler_entry:
    stp x2,  x3,  [sp, #16]
    stp x4,  x5,  [sp, #32]
    stp x6,  x7,  [sp, #48]
    stp x8,  x9,  [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x19, [sp, #144]
    stp x20, x21, [sp, #160]
    stp x22, x23, [sp, #176]
    stp x24, x25, [sp, #192]
    stp x26, x27, [sp, #208]
    stp x28, x29, [sp, #224]
    str x30,      [sp, #240]

    mrs x1, elr_el1
    mrs x2, spsr_el1
    mrs x3, far_el1
    mrs x4, esr_el1
    stp x1, x2, [sp, #256]
    stp x3, x4, [sp, #272]

    mov x0, sp
    bl trap_handler

    ldp x1, x2, [sp, #256]
    msr elr_el1, x1
    msr spsr_el1, x2

    ldp x2,  x3,  [sp, #16]
    ldp x4,  x5,  [sp, #32]
    ldp x6,  x7,  [sp, #48]
    ldp x8,  x9,  [sp, #64]
    ldp x10, x11, [sp, #80]
    ldp x12, x13, [sp, #96]
    ldp x14, x15, [sp, #112]
    ldp x16, x17, [sp, #128]
    ldp x18, x19, [sp, #144]
    ldp x20, x21, [sp, #160]
    ldp x22, x23, [sp, #176]
    ldp x24, x25, [sp, #192]
    ldp x26, x27, [sp, #208]
    ldp x28, x29, [sp, #224]
    ldr x30,      [sp, #240]
    ldp x0,  x1,  [sp, #0]
    add sp, sp, #288
    eret
