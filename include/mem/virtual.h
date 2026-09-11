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
 * This file path: kernel/include/memory/virtual.h
 */
#ifndef MEM_VIRTUAL_H
#define MEM_VIRTUAL_H 1

#define VHEAP_START         0xFFFFC00000000000ULL
#define BUCKET_COUNT        7

#define KMALLOC_MAGIC_SMALL 0xDEADBEEFCAFEBABEULL
#define KMALLOC_MAGIC_LARGE 0xFEEDFACEBEEFDEADULL

#include <stddef.h>

typedef struct {
    uint64_t magic;
    uint64_t alloc_size;
} kalloc_header_t;

typedef struct free_chunk {
    struct free_chunk *next;
} free_chunk_t;

typedef struct vfree_span {
    uint64_t vaddr;
    size_t pages;
    struct vfree_span *next;
} vfree_span_t;

extern const size_t  BUCKET_SIZES[BUCKET_COUNT];

extern uint64_t      VHEAP_BUMP_PTR;
extern free_chunk_t *BUCKET_FREE_LISTS[BUCKET_COUNT];
extern vfree_span_t *VFREE_SPANS;

void  virtual_init(void);

#endif
