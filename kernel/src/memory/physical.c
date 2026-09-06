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
 * This file path: kernel/src/memory/physical.c
 */
#include <memory/physical.h>
#include <memory.h>
#include <panic.h>

extern memory_info_t MEMORY_INFO;

// #include <limine.h>  // 为了 framebuffer 访问
// extern struct limine_framebuffer_request framebuffer_request;
//
// // 画全屏颜色的辅助函数
// static void set_screen_color(uint32_t color) {
//     struct limine_framebuffer *fb = \
//         framebuffer_request.response->framebuffers[0];
//     if (!fb) return;
//     volatile uint32_t *fb_ptr = fb->address;
//     uint64_t pixel_count = fb->width * fb->height;
//     for (uint64_t i = 0; i < pixel_count; i++) {
//         fb_ptr[i] = color;
//     }
//     // 小延迟
//     for (volatile uint64_t i = 0; i < 100000; i++);
// }

static uint64_t free_list_head = 0;
static uint64_t total_free_pages = 0;

static uint64_t align_up(uint64_t addr, uint64_t align) {
    return (addr + align - 1) & ~(align - 1);
}

static uint64_t align_down(uint64_t addr, uint64_t align) {
    return addr & ~(align - 1);
}

void physical_memory_init(void) {
    extern memory_info_t MEMORY_INFO;

    free_list_head = 0;
    total_free_pages = 0;

    for (uint64_t i = 0; i < MEMORY_INFO.region_count; i++) {
        memory_region_t *region = &MEMORY_INFO.regions[i];

        if (!region->usable) continue;

        uint64_t start = align_up(region->base, PAGE_SIZE);
        uint64_t end = align_down(region->base + region->length, PAGE_SIZE);

        for (uint64_t phys = start; phys < end; phys += PAGE_SIZE) {
            volatile uint64_t *next_ptr = (uint64_t *)(phys + MEMORY_INFO.offset);
            *next_ptr = free_list_head;
            free_list_head = phys;
            total_free_pages++;
        }
    }

    if (free_list_head == 0) {
        panic();
    }
}

uint64_t physical_page_alloc(void) {
    if (free_list_head == 0) {
        return 0;
    }

    uint64_t phys = free_list_head;

    volatile uint64_t *next_ptr = (uint64_t *)(phys + MEMORY_INFO.offset);
    free_list_head = *next_ptr;

    total_free_pages--;
    return phys;
}

void physical_page_free(uint64_t phys) {
    if (phys == 0) return;
    if (phys % PAGE_SIZE != 0) return;

    volatile uint64_t *next_ptr = (uint64_t *)(phys + MEMORY_INFO.offset);
    *next_ptr = free_list_head;
    free_list_head = phys;

    total_free_pages++;
}

uint64_t physical_memory_get_usable(void) {
    return total_free_pages * PAGE_SIZE;
}
