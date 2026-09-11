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
 * This file path: kernel/arch/loongarch64/int/trap.S
 */
.section .text

.extern trap_handler

.global trap_vector_table
.global trap_handler_entry
.balign 8
trap_vector_table:
trap_handler_entry:
    addi.d $sp, $sp, -288
    st.d $ra,  $sp, 0
    st.d $tp,  $sp, 8
    st.d $r4,  $sp, 16
    st.d $r5,  $sp, 24
    st.d $r6,  $sp, 32
    st.d $r7,  $sp, 40
    st.d $r8,  $sp, 48
    st.d $r9,  $sp, 56
    st.d $r10, $sp, 64
    st.d $r11, $sp, 72
    st.d $r12, $sp, 80
    st.d $r13, $sp, 88
    st.d $r14, $sp, 96
    st.d $r15, $sp, 104
    st.d $r16, $sp, 112
    st.d $r17, $sp, 120
    st.d $r18, $sp, 128
    st.d $r19, $sp, 136
    st.d $r20, $sp, 144
    st.d $r21, $sp, 152
    st.d $r22, $sp, 160
    st.d $r23, $sp, 168
    st.d $r24, $sp, 176
    st.d $r25, $sp, 184
    st.d $r26, $sp, 192
    st.d $r27, $sp, 200
    st.d $r28, $sp, 208
    st.d $r29, $sp, 216
    st.d $r30, $sp, 224
    st.d $r31, $sp, 232

    csrrd $r4, 0x6
    csrrd $r5, 0x1
    csrrd $r6, 0x5
    csrrd $r7, 0x7
    st.d $r4, $sp, 240
    st.d $r5, $sp, 248
    st.d $r6, $sp, 256
    st.d $r7, $sp, 264

    addi.d $r8, $sp, 288
    st.d $r8, $sp, 272

    move $r4, $sp
    bl trap_handler

    ld.d $r4, $sp, 240
    ld.d $r5, $sp, 248
    csrwr $r4, 0x6
    csrwr $r5, 0x1

    ld.d $ra,  $sp, 0
    ld.d $tp,  $sp, 8
    ld.d $r4,  $sp, 16
    ld.d $r5,  $sp, 24
    ld.d $r6,  $sp, 32
    ld.d $r7,  $sp, 40
    ld.d $r8,  $sp, 48
    ld.d $r9,  $sp, 56
    ld.d $r10, $sp, 64
    ld.d $r11, $sp, 72
    ld.d $r12, $sp, 80
    ld.d $r13, $sp, 88
    ld.d $r14, $sp, 96
    ld.d $r15, $sp, 104
    ld.d $r16, $sp, 112
    ld.d $r17, $sp, 120
    ld.d $r18, $sp, 128
    ld.d $r19, $sp, 136
    ld.d $r20, $sp, 144
    ld.d $r21, $sp, 152
    ld.d $r22, $sp, 160
    ld.d $r23, $sp, 168
    ld.d $r24, $sp, 176
    ld.d $r25, $sp, 184
    ld.d $r26, $sp, 192
    ld.d $r27, $sp, 200
    ld.d $r28, $sp, 208
    ld.d $r29, $sp, 216
    ld.d $r30, $sp, 224
    ld.d $r31, $sp, 232
    addi.d $sp, $sp, 288
    ertn
