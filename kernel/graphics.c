// SPDX-License-Identifier: MIT
/*
 * kernel/graphics.c
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
 * Provides functions for graphic drawing
 *
 */

#include <stdint.h>
#include <boot/bootboot.h>
#include <kernel/graphics.h>
#include <kernel/psf.h>

extern BOOTBOOT bootboot;               // Infomation provided by BOOTBOOT Loader
extern uint8_t fb;                      // linear framebuffer mapped

void putpixel(int x, int y, PIXEL pixel)
{
    *(uint8_t *)(fb + y * bootboot.fb_scanline + x) = pixel;
}

void drawchar(char c, int cx, int cy, PIXEL fg, PIXEL bg)
{
    PSF_font *font = (PSF_font *)&_binary_font_psf_start;

    int bytesperline = (font->width + 7) / 8;
    uint8_t *glyph = (uint8_t *)&_binary_font_psf_start + font->headersize + (c > 0 && c < font->numglyph ? c : 0) * font->bytesperglyph;

    int offs =
        (cy * font->height * bootboot.fb_scanline) +
        (cx * (font->width + 1) * sizeof(PIXEL));
    /* finally display pixels according to the bitmap */
    int x, y, line, mask;
    for (y = 0; y < font->height; y++)
    {
        /* save the starting position of the line */
        line = offs;
        mask = 1 << (font->width - 1);
        /* display a row */
        for (x = 0; x < font->width; x++)
        {
            *((PIXEL *)(&fb + line)) = *((unsigned int *)glyph) & mask ? fg : bg;
            /* adjust to the next pixel */
            mask >>= 1;
            line += sizeof(PIXEL);
        }
        /* adjust to the next line */
        glyph += bytesperline;
        offs += bootboot.fb_scanline;
    }
}