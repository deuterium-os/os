// SPDX-License-Identifier: MIT
/*
 * kernel/kernel.c
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
 * The entry point of kernel. loaded by BOOTBOOT Loader.
 *
 */

#include <stdint.h>
#include <asm/io.h>
#include <boot/bootboot.h>
#include <kernel/graphics.h>
#include <kernel/interrupt.h>
#include <kernel/serial.h>
#include <kernel/tty.h>
#include <kernel/kprintf.h>

extern BOOTBOOT bootboot;               // Infomation provided by BOOTBOOT Loader
extern unsigned char environment[4096]; // configuration, UTF-8 text key=value pairs

void varg_test_b(va_list *parg)
{
    int a = va_arg(*parg, int);
}

void varg_test(int a, ...)
{
    va_list b;
    va_start(b, a);
    varg_test_b(&b);
    va_end(b);
}

/* Entry point, called by BOOTBOOT Loader */
void _start()
{
    int a = sizeof(char*);
    interrupt_init();
    terminal_init();
    terminal_puts("Hello world!\n");
    serial_send_str(PORT_COM1, "Hello world!");
    varg_test(123, 456);
    kprintf("Hello kprintf!\n");
    kprintf("Test: %12.9d Testend\n", 1234567);
    hlt();
}