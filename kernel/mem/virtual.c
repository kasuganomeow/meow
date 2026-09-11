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
 * This file path: kernel/mem/virtual.c
 */
#include <mem/virtual.h>

const size_t  BUCKET_SIZES[BUCKET_COUNT] = \
    {32, 64, 128, 256, 512, 1024, 2048};

uint64_t      VHEAP_BUMP_PTR                  = VHEAP_START;
free_chunk_t *BUCKET_FREE_LISTS[BUCKET_COUNT] = {0};
vfree_span_t *VFREE_SPANS                     = NULL;

void virtual_init(void) {
    VHEAP_BUMP_PTR = VHEAP_START;
    VFREE_SPANS    = NULL;
    for (int32_t index = 0; index < BUCKET_COUNT; index++) {
        BUCKET_FREE_LISTS[index] = NULL;
    }
}
