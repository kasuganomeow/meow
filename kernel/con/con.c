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
 * This file path: kernel/con/con.c
 */
#include <con/con.h>

#include <con/font.h>
#include <con/psf.h>
#include <sync/spinlock.h>
#include <string.h>

extern uint8_t __kcon_font_start[];
extern uint8_t __kcon_font_end[];

static spinlock_t CONSOLE_LOCK = SPINLOCK_INIT;

static console_t  CONSOLE;

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

static void con_scroll(void) {
    size_t pitch_in_pixels = CONSOLE.fb->pitch / 4;
    size_t lines_to_move = CONSOLE.fb->height - FONT.font_height;

    memmove(
        CONSOLE.buffer,
        CONSOLE.buffer +
        (FONT.font_height * pitch_in_pixels),
        lines_to_move * CONSOLE.fb->pitch
    );

    uint32_t *bottom_line = \
        CONSOLE.buffer + (lines_to_move * pitch_in_pixels);
    size_t bottom_pixels = FONT.font_height * pitch_in_pixels;
    for (size_t i = 0; i < bottom_pixels; i++) {
        bottom_line[i] = CONSOLE.bg_color;
    }

    CONSOLE.cursor_y -= FONT.font_height;
}

static void con_drawchar(char c, size_t x, size_t y) {
    uint8_t *glyph = \
        FONT.glyph_base + \
        ((unsigned char)c * FONT.bytes_per_glyph);
    size_t pitch_in_pixels = CONSOLE.fb->pitch / 4;

    for (uint32_t row = 0; row < FONT.font_height; row++) {
        for (uint32_t col = 0; col < FONT.font_width; col++) {
            uint8_t byte = \
                glyph[row * FONT.bytes_per_line + (col / 8)];
            bool is_pixel_on = (byte >> (7 - (col % 8))) & 1;

            uint32_t color = \
                is_pixel_on ? CONSOLE.fg_color : CONSOLE.bg_color;
            CONSOLE.buffer[(y + row) * pitch_in_pixels + (x + col)] = \
                color;
        }
    }
}

static void con_putchar_unlocked(char c) {
    if (c == '\n') {
        CONSOLE.cursor_x = 0;
        CONSOLE.cursor_y += FONT.font_height;
    } else if (c == '\r') {
        CONSOLE.cursor_x = 0;
    } else if (c == '\t') {
        size_t tab_width = FONT.font_width * 4;
        CONSOLE.cursor_x = \
            ((CONSOLE.cursor_x + tab_width) / tab_width) * tab_width;
    } else {
        con_drawchar(c, CONSOLE.cursor_x, CONSOLE.cursor_y);
        CONSOLE.cursor_x += FONT.font_width;
    }

    if (
        CONSOLE.cursor_x + FONT.font_width >
        CONSOLE.fb->width
    ) {
        CONSOLE.cursor_x = 0;
        CONSOLE.cursor_y += FONT.font_height;
    }

    if (
        CONSOLE.cursor_y + FONT.font_height >
        CONSOLE.fb->height
    ) {
        con_scroll();
    }
}

bool con_is_ready(void) {
    return CONSOLE.ready;
}

void con_init(void) {
    spinlock_acquire(&CONSOLE_LOCK);

    struct limine_framebuffer *fb = \
        framebuffer_request.response->framebuffers[0];

    CONSOLE.fb = fb;
    CONSOLE.buffer = (uint32_t *)fb->address;
    CONSOLE.cursor_x = 0;
    CONSOLE.cursor_y = 0;
    CONSOLE.fg_color = 0x00FFFFFF;
    CONSOLE.bg_color = 0x00000000;

    size_t total_pixels = (fb->pitch / 4) * fb->height;
    for (size_t index = 0; index < total_pixels; index++) {
        CONSOLE.buffer[index] = CONSOLE.bg_color;
    }

    CONSOLE.ready = true;

    spinlock_release(&CONSOLE_LOCK);
}

void con_putchar(char c) {
    spinlock_acquire(&CONSOLE_LOCK);

    con_putchar_unlocked(c);

    spinlock_release(&CONSOLE_LOCK);
}

void con_write(const char *str) {
    if (!str) return;

    spinlock_acquire(&CONSOLE_LOCK);

    while (*str) {
        con_putchar_unlocked(*str++);
    }

    spinlock_release(&CONSOLE_LOCK);
}

void con_set_color(uint32_t fg, uint32_t bg) {
    CONSOLE.fg_color = fg;
    CONSOLE.bg_color = bg;
}
