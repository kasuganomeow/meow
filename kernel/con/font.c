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
 * This file path: kernel/con/font.c
 */
#include <con/font.h>

#include <con/psf.h>

extern uint8_t __kcon_font_start[];
extern uint8_t __kcon_font_end[];

font_t FONT;

void font_init(void) {
    uint8_t *font_bin = (uint8_t *)__kcon_font_start;
    psf2_header_t *psf2 = (psf2_header_t *)font_bin;
    psf1_header_t *psf1 = (psf1_header_t *)font_bin;

    if (psf2->magic == PSF2_MAGIC) {
        FONT.font_width = psf2->width;
        FONT.font_height = psf2->height;
        FONT.bytes_per_glyph = psf2->bytes_per_glyph;
        FONT.bytes_per_line = (psf2->width + 7) / 8;
        FONT.glyph_base = font_bin + psf2->header_size;
    } else if (
        psf1->magic[0] == PSF1_MAGIC0 &&
        psf1->magic[1] == PSF1_MAGIC1
    ) {
        FONT.font_width = 8;
        FONT.font_height = psf1->charsize;
        FONT.bytes_per_glyph = psf1->charsize;
        FONT.bytes_per_line = 1;
        FONT.glyph_base = font_bin + sizeof(psf1_header_t);
    } else {
        FONT.font_width = 8;
        FONT.font_height = 16;
        FONT.bytes_per_glyph = 16;
        FONT.bytes_per_line = 1;
        FONT.glyph_base = font_bin;
    }
}
