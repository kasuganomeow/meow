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
 * This file path: kernel/mem/xalloc.c
 */
#include <mem/xalloc.h>

#include <mem/kalloc.h>
#include <mem/mem.h>
#include <mem/physical.h>
#include <mem/virtual.h>
#include <mmu/mapping.h>
#include <mmu/mmu.h>
#include <mmu/paging.h>
#include <panic.h>
#include <sync/spinlock.h>

static spinlock_t PALLOC_LOCK = SPINLOCK_INIT;
static spinlock_t VALLOC_LOCK = SPINLOCK_INIT;

uint64_t palloc(void) {
    if (FREE_LIST_HEAD == 0) return 0;

    spinlock_acquire(&PALLOC_LOCK);

    uint64_t phys = FREE_LIST_HEAD;

    volatile uint64_t *next_ptr = \
        (uint64_t *)(phys + MEMORY_INFO.hhdm_offset);
    FREE_LIST_HEAD = *next_ptr;

    TOTAL_FREE_PAGES--;

    spinlock_release(&PALLOC_LOCK);

    return phys;
}

void pfree(uint64_t phys) {
    if (phys == 0 || phys % PMM_PAGE_SIZE != 0) return;

    spinlock_acquire(&PALLOC_LOCK);

    volatile uint64_t *next_ptr = \
        (uint64_t *)(phys + MEMORY_INFO.hhdm_offset);
    *next_ptr = FREE_LIST_HEAD;
    FREE_LIST_HEAD = phys;

    TOTAL_FREE_PAGES++;

    spinlock_release(&PALLOC_LOCK);
}

void *valloc(size_t pages) {
    if (pages == 0) return NULL;

    spinlock_acquire(&VALLOC_LOCK);

    uint64_t target_vaddr = 0;

    vfree_span_t **prev = &VFREE_SPANS;
    vfree_span_t  *curr =  VFREE_SPANS;
    while (curr != NULL) {
        if (curr->pages >= pages) {
            target_vaddr = curr->vaddr;
            if (curr->pages == pages) {
                *prev = curr->next;
                kfree(curr);
            } else {
                curr->vaddr += pages * PAGE_SIZE;
                curr->pages -= pages;
            }
            break;
        }
        prev = &curr->next;
        curr = curr->next;
    }

    if (target_vaddr == 0) {
        target_vaddr = VHEAP_BUMP_PTR;
        VHEAP_BUMP_PTR += pages * PAGE_SIZE;
    }

    for (size_t i = 0; i < pages; i++) {
        uint64_t phys = palloc();
        if (phys == 0) {
            panic("valloc: Out of physical memory (palloc failed at page %zu of %zu for virtual base %p, total: %zu KB)",
                  i + 1, pages, (void *)target_vaddr, (pages * PAGE_SIZE) / 1024);
        }

        uint64_t vpage = target_vaddr + i * PAGE_SIZE;
        mapping_page(
            &KERNEL_PAGEMAP,
            vpage, phys,
            PAGE_READABLE | PAGE_WRITABLE
        );
    }

    spinlock_release(&VALLOC_LOCK);

    return (void *)target_vaddr;
}

void vfree(void *ptr, size_t pages) {
    if (ptr == NULL || pages == 0) return;

    spinlock_acquire(&VALLOC_LOCK);

    uint64_t vaddr = (uint64_t)ptr;

    for (size_t i = 0; i < pages; i++) {
        uint64_t vpage = vaddr + i * PAGE_SIZE;
        uint64_t phys  = mmu_get_phys(&KERNEL_PAGEMAP, vpage);

        if (phys != 0) {
            pfree(phys);
        }
        unmapping_page(&KERNEL_PAGEMAP, vpage);
    }

    vfree_span_t *span = (vfree_span_t *)kalloc(sizeof(vfree_span_t));
    if (span != NULL) {
        span->vaddr = vaddr;
        span->pages = pages;
        span->next  = VFREE_SPANS;
        VFREE_SPANS = span;
    }

    spinlock_release(&VALLOC_LOCK);
}
