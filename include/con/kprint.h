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
 * This file path: include/con/kprint.h
 */
#ifndef CON_KPRINTF_H
#define CON_KPRINTF_H 1

#define LINE_BUFFER_SIZE 1024
#define LOG_RING_SIZE    32768

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char   buffer[LOG_RING_SIZE];
    size_t head;
    size_t read;
    size_t count;
} kprint_t;

void    kprint_flush(void);
int32_t vsnprint(char *buf, size_t size, const char *fmt, va_list args);
int32_t snprint(char *buf, size_t size, const char *fmt, ...);
int32_t kvprint(const char *fmt, va_list args);
int32_t kprint(const char *fmt, ...);

#endif
