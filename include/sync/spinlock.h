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
 * This file path: kernel/include/sync/spinlock.h
 */
#ifndef SYNC_SPINLOCK_H
#define SYNC_SPINLOCK_H 1

#include <stdint.h>
#include <stdbool.h>
#include <cpu/cpu.h>

typedef struct {
    volatile uint32_t lock;
} spinlock_t;

#define SPINLOCK_INIT ((spinlock_t){ .lock = 0 })

static inline void spinlock_init(spinlock_t *sl) {
    __atomic_store_n(&sl->lock, 0, __ATOMIC_RELAXED);
}

static inline bool spinlock_try_acquire(spinlock_t *sl) {
    uint32_t expected = 0;
    return __atomic_compare_exchange_n(&sl->lock, &expected, 1,
                                       false,
                                       __ATOMIC_ACQUIRE,
                                       __ATOMIC_RELAXED);
}

static inline void spinlock_acquire(spinlock_t *sl) {
    while (!spinlock_try_acquire(sl)) {
        while (__atomic_load_n(&sl->lock, __ATOMIC_RELAXED) == 1) {
            spin();
        }
    }
}

static inline void spinlock_release(spinlock_t *sl) {
    __atomic_store_n(&sl->lock, 0, __ATOMIC_RELEASE);
}

#endif
