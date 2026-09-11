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
 * This file path: kernel/mem/kalloc.c
 */
#include <mem/kalloc.h>

#include <mem/mem.h>
#include <mem/virtual.h>
#include <mem/xalloc.h>
#include <mmu/paging.h>
#include <panic.h>
#include <stddef.h>
#include <string.h>
#include <sync/spinlock.h>

static spinlock_t KALLOC_LOCK = SPINLOCK_INIT;

static int32_t get_bucket_index(size_t size) {
    for (int32_t index = 0; index < BUCKET_COUNT; index++) {
        if (size <= BUCKET_SIZES[index]) {
            return index;
        }
    }
    return -1;
}

void *kalloc(size_t size) {
    if (size == 0) return NULL;
    spinlock_acquire(&KALLOC_LOCK);

    size_t total_size = size + sizeof(kalloc_header_t);
    int32_t bucket_idx = get_bucket_index(total_size);

    if (bucket_idx == -1) {
        size_t pages = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;
        kalloc_header_t *header = (kalloc_header_t *)valloc(pages);

        spinlock_release(&KALLOC_LOCK);

        if (!header) return NULL;

        header->magic = KMALLOC_MAGIC_LARGE;
        header->alloc_size = pages;

        return (void *)(header + 1);
    }

    size_t chunk_size = BUCKET_SIZES[bucket_idx];

    if (BUCKET_FREE_LISTS[bucket_idx] == NULL) {
        uint64_t new_page_phys = palloc();
        if (!new_page_phys) {
            panic("kalloc: Out of physical memory (failed to allocate slab page for bucket size %zu)", chunk_size);
        }

        uint8_t *page_virt = (uint8_t *)(new_page_phys + MEMORY_INFO.hhdm_offset);
        size_t count = PAGE_SIZE / chunk_size;

        for (size_t i = 0; i < count; i++) {
            free_chunk_t *chunk = (free_chunk_t *)(page_virt + i * chunk_size);
            chunk->next = BUCKET_FREE_LISTS[bucket_idx];
            BUCKET_FREE_LISTS[bucket_idx] = chunk;
        }
    }

    free_chunk_t *chunk = BUCKET_FREE_LISTS[bucket_idx];
    BUCKET_FREE_LISTS[bucket_idx] = chunk->next;

    kalloc_header_t *header = (kalloc_header_t *)chunk;
    header->magic = KMALLOC_MAGIC_SMALL;
    header->alloc_size = chunk_size;

    spinlock_release(&KALLOC_LOCK);
    return (void *)(header + 1);
}

void kfree(void *ptr) {
    if (ptr == NULL) return;
    spinlock_acquire(&KALLOC_LOCK);

    kalloc_header_t *header = ((kalloc_header_t *)ptr) - 1;

    if (header->magic == KMALLOC_MAGIC_SMALL) {
        size_t chunk_size = header->alloc_size;
        int bucket_idx = get_bucket_index(chunk_size);
        if (bucket_idx == -1) {
            panic("kfree: Slab heap corruption detected (invalid chunk size %zu for pointer %p)", chunk_size, ptr);
        }

        header->magic = 0;

        free_chunk_t *chunk = (free_chunk_t *)header;
        chunk->next = BUCKET_FREE_LISTS[bucket_idx];
        BUCKET_FREE_LISTS[bucket_idx] = chunk;

    } else if (header->magic == KMALLOC_MAGIC_LARGE) {
        size_t pages = header->alloc_size;
        header->magic = 0;
        vfree(header, pages);
    } else {
        panic("kfree: Invalid or double-freed pointer %p (header magic: 0x%016lx)", ptr, header->magic);
    }

    spinlock_release(&KALLOC_LOCK);
}

void *kcalloc(size_t num, size_t size) {
    size_t total = num * size;
    void *ptr = kalloc(total);
    if (ptr != NULL) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *krealloc(void *ptr, size_t new_size) {
    if (ptr == NULL) return kalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    kalloc_header_t *header = ((kalloc_header_t *)ptr) - 1;
    size_t old_usable_size = 0;

    if (header->magic == KMALLOC_MAGIC_SMALL) {
        old_usable_size = header->alloc_size - sizeof(kalloc_header_t);
    } else if (header->magic == KMALLOC_MAGIC_LARGE) {
        old_usable_size = (header->alloc_size * PAGE_SIZE) - sizeof(kalloc_header_t);
    } else {
        panic("krealloc: Invalid pointer %p (corrupted or already freed, header magic: 0x%016lx)", ptr, header->magic);
    }

    if (new_size <= old_usable_size) {
        return ptr;
    }

    void *new_ptr = kalloc(new_size);
    if (new_ptr != NULL) {
        memcpy(new_ptr, ptr, old_usable_size);
        kfree(ptr);
    }
    return new_ptr;
}
