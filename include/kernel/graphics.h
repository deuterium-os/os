// SPDX-License-Identifier: MIT
/*
 * include/kernel/graphics.h
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <boot/bootboot.h>

typedef uint32_t PIXEL; /* pixel pointer */

void putpixel(int x, int y, PIXEL pixel);

void drawchar(char c, int cx, int cy, PIXEL fg, PIXEL bg);

/* Converts 24-bit RGB color(like 0xRRGGBB) to 32-bit Pixel format according to the FrameBuffer */
static inline PIXEL rgb_to_pixel(uint32_t rgb)
{
    extern BOOTBOOT bootboot; // Infomation provided by BOOTBOOT Loader

    switch (bootboot.fb_type)
    {
    default:
    case FB_ARGB:
        return (0xFF << 24) | rgb;

    case FB_RGBA:
        return (rgb << 8) | 0xFF;

    case FB_ABGR:
        return (0xFF << 24) | ((rgb & 0xFF) << 16) | (rgb & 0xFF00) | ((rgb & 0xFF0000) >> 16);

    case FB_BGRA:
        return ((rgb & 0xFF) << 24) | ((rgb & 0xFF00) << 8) | ((rgb & 0xFF0000) >> 8) | 0xFF;
    }
}


#endif/* GRAPHICS_H */
