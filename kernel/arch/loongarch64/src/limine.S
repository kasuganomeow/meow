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
 * This file path: kernel/arch/loongarch64/src/stopcpu.S
 */
.section .limine_requests_start, "aw", @progbits
.align 8
.global limine_requests_start_marker
limine_requests_start_marker:
    .quad 0xf6b8f4b39de7d1ae
    .quad 0xfab91a6940fcb9cf
    .quad 0x785c6ed015d3e316
    .quad 0x181e920a7852b9d9

.section .limine_requests, "aw", @progbits
.align 8
.global limine_base_revision
limine_base_revision:
    .quad 0xf9562b2d5c95a6c8
    .quad 0x6a7b384944536bdc
    .quad 6

.global framebuffer_request
framebuffer_request:
    .quad 0xc7b1dd30df4c8b88
    .quad 0x0a82e883a194f07b
    .quad 0x9d5827dcd881dd75
    .quad 0xa3148604f6fab11b
    .quad 0
    .quad 0

.global memmap_request
memmap_request:
    .quad 0xc7b1dd30df4c8b88
    .quad 0x0a82e883a194f07b
    .quad 0x67cf3d9d378a806f
    .quad 0xe304acdfc50c3c62
    .quad 0
    .quad 0

.global hhdm_request
hhdm_request:
    .quad 0xc7b1dd30df4c8b88
    .quad 0x0a82e883a194f07b
    .quad 0x48dcf1cb8ad2b852
    .quad 0x63984e959a98244b
    .quad 0
    .quad 0

.section .limine_requests_end, "aw", @progbits
.align 8
.global limine_requests_end_marker
limine_requests_end_marker:
    .quad 0xadc0e0531bb10d03
    .quad 0x9572709f31764c62
