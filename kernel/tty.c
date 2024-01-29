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
#include <kernel/tty.h>
#include <stdint.h>
#include <stddef.h>
#include <boot/bootboot.h>

size_t cursor_x;
size_t cursor_y;

PIXEL terminal_fgcolor;
PIXEL terminal_bgcolor;

extern uint8_t fb;              // linear framebuffer mapped

void terminal_init()
{
    cursor_x = 0;
    cursor_y = 0;

    terminal_setcolor(rgb_to_pixel(0xAAAAAA), rgb_to_pixel(0x000000));
}

void terminal_setcolor(PIXEL fg, PIXEL bg)
{
    terminal_fgcolor = fg;
    terminal_bgcolor = bg;
}

void terminal_putchar(char c)
{
    drawchar(c, cursor_x, cursor_y, terminal_fgcolor, terminal_bgcolor);
    cursor_x++;
}

void terminal_puts(char *s)
{
    while (*s)
    {
        terminal_putchar(*s);
        s++;
    }
}