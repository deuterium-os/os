// SPDX-License-Identifier: MIT
/*
 * arch/x86/kernel/serial.c
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
 * Functions of serial ports
 *
 */

#include <stdint.h>
#include <asm/io.h>
#include <kernel/serial.h>

uint8_t serial_recv_byte(uint16_t port)
{
    while ((inb(port + PORT_OFFSET_LSR) & 1) == 0); // Wait for the receive buffer has readable data
    return inb(port + PORT_OFFSET_DR);
}

void serial_send_byte(uint16_t port, uint8_t data)
{
    while ((inb(port + PORT_OFFSET_LSR) & 0x20) == 0); // Wait for the transmit buffer is empty
    outb(port + PORT_OFFSET_DR, data);
}

int serial_recv_str(uint16_t port, char *buffer)
{
    int received_bytes = 0;
    while ((inb(port + PORT_OFFSET_LSR) & 1) == 0); // Wait for the receive buffer has readable data
    do
    {
        *buffer = inb(port + PORT_OFFSET_DR);
        received_bytes++;
    } while (*(buffer++));
    return received_bytes;
}

int serial_send_str(uint16_t port, char *str)
{
    int sent_bytes = 0;
    while ((inb(port + PORT_OFFSET_LSR) & 0x20) == 0); // Wait for the transmit buffer is empty
    do
    {
        outb(port + PORT_OFFSET_DR, *str);
    } while (*(str++));
    return sent_bytes;
}