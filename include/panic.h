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
 * This file path: include/core/panic.h
 */
#ifndef PANIC_H
#define PANIC_H 1

[[gnu::noreturn]]
void panic_impl
(
    const char *file,
    int line,
    const char *func,
    const char *fmt, ...
);

#define panic(fmt, ...) \
    panic_impl(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#endif
