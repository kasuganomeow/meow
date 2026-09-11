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
 * This file path: kernel/con/kprint.c
 */
#include <con/kprint.h>

#include <con/con.h>
#include <sync/spinlock.h>

static kprint_t KPRINT = {
    .head  = 0,
    .read  = 0,
    .count = 0
};

spinlock_t KPRINT_LOCK = SPINLOCK_INIT;

static void push_char(char c) {
    KPRINT.buffer[KPRINT.head] = c;
    KPRINT.head = (KPRINT.head + 1) % LOG_RING_SIZE;
    if (KPRINT.count < LOG_RING_SIZE) {
        KPRINT.count++;
    }
}

static void flush_to_fb(void) {
    if (!con_is_ready()) {
        return;
    }

    char tmp[LINE_BUFFER_SIZE];
    size_t i = 0;

    while (KPRINT.read != KPRINT.head && i < sizeof(tmp) - 1) {
        tmp[i++] = KPRINT.buffer[KPRINT.read];
        KPRINT.read = (KPRINT.read + 1) % LOG_RING_SIZE;
    }
    tmp[i] = '\0';

    if (i > 0) {
        con_write(tmp);
    }
}

static int32_t number_to_string(
    char *buf, size_t size, uint64_t num, int base, bool uppercase,
    int width, bool pad_zero, bool is_negative
) {
    char tmp[65];
    int digits = 0;
    const char *digits_map = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (num == 0) {
        tmp[digits++] = '0';
    } else {
        while (num > 0) {
            tmp[digits++] = digits_map[num % base];
            num /= base;
        }
    }

    int total_len = digits + (is_negative ? 1 : 0);
    int pad_len = (width > total_len) ? (width - total_len) : 0;
    int written = 0;

    if (is_negative && pad_zero) {
        if (written < (int)size - 1) buf[written++] = '-';
        is_negative = false;
    }

    while (pad_len-- > 0) {
        if (written < (int)size - 1) {
            buf[written++] = pad_zero ? '0' : ' ';
        }
    }

    if (is_negative) {
        if (written < (int)size - 1) buf[written++] = '-';
    }

    while (--digits >= 0) {
        if (written < (int)size - 1) {
            buf[written++] = tmp[digits];
        }
    }

    return written;
}

void kprint_flush (void) {
    spinlock_acquire(&KPRINT_LOCK);
    flush_to_fb();
    spinlock_release(&KPRINT_LOCK);
}

int vsnprint(char *buf, size_t size, const char *fmt, va_list args) {
    if (!buf || size == 0) return 0;

    size_t written = 0;

    while (*fmt && written < size - 1) {
        if (*fmt != '%') {
            buf[written++] = *fmt++;
            continue;
        }

        fmt++;

        bool pad_zero = false;
        if (*fmt == '0') {
            pad_zero = true;
            fmt++;
        }

        int width = 0;
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        int length_mod = 0;
        if (*fmt == 'l') {
            length_mod = 1;
            fmt++;
            if (*fmt == 'l') {
                length_mod = 2;
                fmt++;
            }
        } else if (*fmt == 'z') {
            length_mod = 2;
            fmt++;
        }

        switch (*fmt) {
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                while (*s && written < size - 1) {
                    buf[written++] = *s++;
                }
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                buf[written++] = c;
                break;
            }
            case 'd':
            case 'i': {
                int64_t val;
                if (length_mod == 2) {
                    val = va_arg(args, int64_t);
                } else if (length_mod == 1) {
                    val = va_arg(args, long);
                } else {
                    val = va_arg(args, int);
                }

                bool is_neg = (val < 0);
                uint64_t uval = is_neg ? (uint64_t)(-val) : (uint64_t)val;

                written += number_to_string
                (
                    buf + written, size - written, uval, 10, false,
                    width, pad_zero, is_neg
                );
                break;
            }
            case 'u': {
                uint64_t val =               \
                    (length_mod >= 1) ?      \
                    va_arg(args, uint64_t) : \
                    va_arg(args, unsigned int);
                written += number_to_string
                (
                    buf + written, size - written, val, 10, false,
                    width, pad_zero, false
                );
                break;
            }
            case 'x': {
                uint64_t val =               \
                    (length_mod >= 1) ?      \
                    va_arg(args, uint64_t) : \
                    va_arg(args, unsigned int);
                written += number_to_string
                (
                    buf + written, size - written, val, 16, false,
                    width, pad_zero, false
                );
                break;
            }
            case 'X': {
                uint64_t val =               \
                    (length_mod >= 1) ?      \
                    va_arg(args, uint64_t) : \
                    va_arg(args, unsigned int);
                written += number_to_string
                (
                    buf + written, size - written, val, 16, true,
                    width, pad_zero, false
                );
                break;
            }
            case 'p': {
                uint64_t val = (uint64_t)va_arg(args, void *);
                if (written + 2 < size - 1) {
                    buf[written++] = '0';
                    buf[written++] = 'x';
                }
                written += number_to_string
                (
                    buf + written, size - written, val, 16, false,
                    16, true, false
                );
                break;
            }
            case 'b': {
                uint64_t val =               \
                    (length_mod >= 1) ?      \
                    va_arg(args, uint64_t) : \
                    va_arg(args, unsigned int);
                written += number_to_string
                (
                    buf + written, size - written, val, 2, false,
                    width, pad_zero, false
                );
                break;
            }
            case '%': {
                buf[written++] = '%';
                break;
            }
            default: {
                buf[written++] = '%';
                if (*fmt) buf[written++] = *fmt;
                break;
            }
        }
        fmt++;
    }

    buf[written] = '\0';
    return written;
}

int32_t snprint(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprint(buf, size, fmt, args);
    va_end(args);
    return ret;
}

int32_t kvprint(const char *fmt, va_list args) {
    char line[LINE_BUFFER_SIZE];
    int len = vsnprint(line, sizeof(line), fmt, args);

    spinlock_acquire(&KPRINT_LOCK);

    for (int i = 0; i < len; i++) {
        push_char(line[i]);
    }

    flush_to_fb();

    spinlock_release(&KPRINT_LOCK);

    return len;
}

int32_t kprint(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = kvprint(fmt, args);
    va_end(args);
    return ret;
}
