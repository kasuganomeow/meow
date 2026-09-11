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
 * This file path: include/int/trap.h
 */
#ifndef INT_TRAP_H
#define INT_TRAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct trap_frame trap_frame_t;
typedef void (*trap_handler_fn_t)(trap_frame_t *frame);

void      trap_init(void);
void      trap_init_percpu(void);
void      trap_register_handler(uint32_t vector, trap_handler_fn_t handler);
void      trap_handler(trap_frame_t *frame);
uintptr_t trap_frame_get_ip(const trap_frame_t *frame);
uintptr_t trap_frame_get_sp(const trap_frame_t *frame);
bool      trap_frame_is_user(const trap_frame_t *frame);
[[gnu::noreturn]]
void      enter_userspace(uintptr_t user_entry, uintptr_t user_sp);

#endif
