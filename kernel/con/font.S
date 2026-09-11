/* SPDX-License-Identifier: AGPL-3.0-or-later AND GPL-2.0-or-later WITH Font-exception-2.0 */
/* Copyright 2026 KasuganoMeow (font.S assembly wrapper)

   This file is part of the Meow.

   The embedded data ("unifont.psf") is part of the GNU Unifont
   project and is licensed under GPL-2.0-or-later WITH
   Font-exception-2.0. Copyright and license information for GNU
   Unifont is retained in kernel/kcon/LICENSE; see that
   file for the full license text.

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
 * This file path: kernel/con/font.S
 */
.section .rodata

.global __kcon_font_start
.global __kcon_font_end
__kcon_font_start:
    .incbin "unifont.psf"
__kcon_font_end:
