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
 * This file path: kernel/arch/riscv64/int/trap.S
 */
.section .text

.extern trap_handler

.global trap_vector_table
.global trap_handler_entry
.balign 4
trap_vector_table:
trap_handler_entry:
    addi sp, sp, -288
    sd ra,  0(sp)
    sd gp,  8(sp)
    sd tp,  16(sp)
    sd t0,  24(sp)
    sd t1,  32(sp)
    sd t2,  40(sp)
    sd s0,  48(sp)
    sd s1,  56(sp)
    sd a0,  64(sp)
    sd a1,  72(sp)
    sd a2,  80(sp)
    sd a3,  88(sp)
    sd a4,  96(sp)
    sd a5,  104(sp)
    sd a6,  112(sp)
    sd a7,  120(sp)
    sd s2,  128(sp)
    sd s3,  136(sp)
    sd s4,  144(sp)
    sd s5,  152(sp)
    sd s6,  160(sp)
    sd s7,  168(sp)
    sd s8,  176(sp)
    sd s9,  184(sp)
    sd s10, 192(sp)
    sd s11, 200(sp)
    sd t3,  208(sp)
    sd t4,  216(sp)
    sd t5,  224(sp)
    sd t6,  232(sp)

    csrr t0, sepc
    csrr t1, sstatus
    csrr t2, scause
    csrr t3, stval
    sd t0, 240(sp)
    sd t1, 248(sp)
    sd t2, 256(sp)
    sd t3, 264(sp)

    addi t4, sp, 288
    sd t4, 272(sp)

    mv a0, sp
    call trap_handler

    ld t0, 240(sp)
    ld t1, 248(sp)
    csrw sepc, t0
    csrw sstatus, t1

    ld ra,  0(sp)
    ld gp,  8(sp)
    ld tp,  16(sp)
    ld t0,  24(sp)
    ld t1,  32(sp)
    ld t2,  40(sp)
    ld s0,  48(sp)
    ld s1,  56(sp)
    ld a0,  64(sp)
    ld a1,  72(sp)
    ld a2,  80(sp)
    ld a3,  88(sp)
    ld a4,  96(sp)
    ld a5,  104(sp)
    ld a6,  112(sp)
    ld a7,  120(sp)
    ld s2,  128(sp)
    ld s3,  136(sp)
    ld s4,  144(sp)
    ld s5,  152(sp)
    ld s6,  160(sp)
    ld s7,  168(sp)
    ld s8,  176(sp)
    ld s9,  184(sp)
    ld s10, 192(sp)
    ld s11, 200(sp)
    ld t3,  208(sp)
    ld t4,  216(sp)
    ld t5,  224(sp)
    ld t6,  232(sp)
    addi sp, sp, 288
    sret
