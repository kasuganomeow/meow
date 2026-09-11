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
 * This file path: include/con/con.h
 */
#ifndef CON_CON_H
#define CON_CON_H 1

#include <stddef.h>
#include <stdint.h>
#include <limine.h>

typedef struct {
    struct limine_framebuffer *fb;
    uint32_t *buffer;
    size_t    cursor_x;
    size_t    cursor_y;
    uint32_t  fg_color;
    uint32_t  bg_color;
    bool      ready;
} console_t;

bool con_is_ready(void);
void con_init(void);
void con_putchar(char c);
void con_write(const char *str);
void con_set_color(uint32_t fg, uint32_t bg);

#endif
