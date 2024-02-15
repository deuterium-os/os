// SPDX-License-Identifier: MIT
/*
 * arch/x86/include/kernel/serial.h
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
 * Definitions and functions of serial ports
 *
 */

#ifndef SERIAL_H
#define SERIAL_H

#define PORT_BASE_COM1 0x3F8
#define PORT_BASE_COM2 0x2F8
#define PORT_BASE_COM3 0x3E8
#define PORT_BASE_COM4 0x2E8
#define PORT_BASE_COM5 0x5F8
#define PORT_BASE_COM6 0x4F8
#define PORT_BASE_COM7 0x5E8
#define PORT_BASE_COM8 0x4E8

#define PORT_OFFSET_DR 0            // Data Register (when DLAB=0)
#define PORT_OFFSET_IER 1           // Interrupt Enable Register (when DLAB=0)
#define PORT_OFFSET_BAUD_LOW 0      // The LSB of the divisor for setting the baud rate (when DLAB=1)
#define PORT_OFFSET_BAUD_HIGH 1     // The MSB of the divisor for setting the baud rate (when DLAB=1)
#define PORT_OFFSET_IIR 2           // Interrupt Identification Register
#define PORT_OFFSET_FCR 2           // FIFO Control Register
#define PORT_OFFSET_LCR 3           // Line Control Register
#define PORT_OFFSET_MCR 4           // Modem Control Register
#define PORT_OFFSET_LSR 5           // Line Status Register
#define PORT_OFFSET_MSR 6           // Modem Status Register
#define PORT_OFFSET_SR 7            // Scratch Register

uint8_t serial_recv(uint16_t port);

void serial_send(uint16_t port, uint8_t data);

int serial_recv_str(uint16_t port, char *buffer);

int serial_send_str(uint16_t port, char *str);

#endif/* SERIAL_H */
