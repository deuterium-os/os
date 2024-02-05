// SPDX-License-Identifier: MIT
/*
 * kernel/tty.c
 *
 * Copyright (c) 2024 CharaDrinkingTea
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Provides functions for terminal input and output
 *
 */

#include <kernel/graphics.h>
#include <kernel/psf.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <stddef.h>
#include <boot/bootboot.h>

size_t cursor_x;
size_t cursor_y;

size_t terminal_width;
size_t Terminal_height;

PIXEL terminal_fgcolor;
PIXEL terminal_bgcolor;

extern BOOTBOOT bootboot; // Infomation provided by BOOTBOOT Loader
extern uint8_t fb;        // linear framebuffer mapped

void terminal_init()
{
    cursor_x = 0;
    cursor_y = 0;

    terminal_setcolor(rgb_to_pixel(0xAAAAAA), rgb_to_pixel(0x000000));

    PSF_font *font = (PSF_font *)&_binary_font_psf_start;
    terminal_width = bootboot.fb_width / (font->width + 1);
    Terminal_height = bootboot.fb_height / font->height;
}

void terminal_setcolor(PIXEL fg, PIXEL bg)
{
    terminal_fgcolor = fg;
    terminal_bgcolor = bg;
}

void terminal_putchar(char c)
{
    if (c != '\n')
    {
        drawchar(c, cursor_x, cursor_y, terminal_fgcolor, terminal_bgcolor);
    }
    cursor_x++;
    if (cursor_x == terminal_width || c == '\n')
    {
        cursor_x = 0;
        cursor_y += 1;
    }
}

void terminal_puts(char *s)
{
    while (*s)
    {
        terminal_putchar(*s);
        s++;
    }
    terminal_putchar('\n');
}

char *itoa(int value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10)
    {
        *ptr++ = '-';
    }
    low = ptr;
    do
    {
        // Trick to make abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}