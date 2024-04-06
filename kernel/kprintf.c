// SPDX-License-Identifier: MIT
/*
 * kernel/kprintf.c
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
 * kprintf series functions and helper functions
 *
 */

#include <stdarg.h>
#include <kernel/tty.h>

enum FORMAT_STATE
{
    INTRODUCE,
    FLAG,
    WIDTH,
    PRECISION,
    SIZE,
    SPECIFIER,
};

enum FORMAT_FLAGS
{
    NONE = 0,
    ALTERNATE = 1,
    ZERO_PADDED = 2,
    LEFT_ALIGN = 4,
    PREFIX_BLANK = 8,
    PREFIX_SIGN = 16
};

enum FORMAT_SIZE
{
    CHAR,
    SHORT,
    LONG,
    LONG_LONG,
    INTMAX,
    SIZE_T,
    PTRDIFF_T,
    LONG_DOUBLE,
};

void print_int(long long value, char **pstr, char type, enum FORMAT_FLAGS flags, int width, int precision)
{
    int radix;
    switch (type)
    {
    case 'd':
    case 'i':
    case 'u':
        radix = 10;
        break;
    case 'o':
        radix = 8;
        break;
    case 'x':
    case 'X':
        radix = 16;
        break;

    default:
        return;
    }

    int length = 0;                 // Length of string, includes prefix
    if (type == 'd' || type == 'i') // signed demical number
    {
        if (value < 0)
        {
            *(*pstr)++ = '-';
            length++;
        }
        else if (flags & PREFIX_SIGN)
        {
            *(*pstr)++ = '+';
            length++;
        }
        else if (flags & PREFIX_BLANK)
        {
            *(*pstr)++ = ' ';
            length++;
        }
    }

    if (flags & ALTERNATE && (radix == 8 || radix == 16) && value)
    {
        *(*pstr)++ = '0';
        length++;
        if (type == 'x')
        {
            *(*pstr)++ = 'x';
            length++;
        }
        else if (type == 'X')
        {
            *(*pstr)++ = 'X';
            length++;
        }
        
    }
    
    char *low = *pstr;

    int digitcount = 0; // Number of digits, don't include prefix
    do
    {
        // The modulo of a negative number is a negative number. This trick makes absolute values unnecessary
        *(*pstr)++ = (type == 'X' ? "FEDCBA9876543210123456789ABCDEF" : "fedcba9876543210123456789abcdef")[15 + value % radix];
        value /= radix;
        digitcount++;
    } while (value);
    length += digitcount;

    // Reverse the string
    char *i = low, *j = *pstr - 1;
    while (i < j)
    {
        char tmp = *i;
        *i++ = *j;
        *j-- = tmp;
    }

    // Add paddings according to width and precision
    int padding_zero_count;
    if (digitcount < precision)
    {
        padding_zero_count = precision - digitcount;
    }
    int padded_length = length + padding_zero_count;
    int padding_blank_count;
    if (padded_length < width)
    {
        if (flags & ZERO_PADDED && !(flags & LEFT_ALIGN))
        {
            padding_zero_count += width - padded_length;
        }
        else
        {
            padding_blank_count = width - padded_length;
        }
    }
    int padding_total_count = padding_zero_count + (flags & LEFT_ALIGN ? 0 : padding_blank_count);
    
    // Move string to place paddings
    char *high = *pstr - 1;
    if (padding_total_count)
    {
        while (high >= low)
        {
            *(high + padding_total_count + 1) = *high--;
        }
    }
    *pstr += padding_total_count;
    
    if (padding_blank_count)
    {
        if (flags & LEFT_ALIGN)
        {
            for (int i = 0; i < padding_blank_count; i++)
            {
                *(*pstr)++ = ' ';
            }
        }
        else
        {
            for (int i = 0; i < padding_blank_count; i++)
            {
                *(low + i) = ' ';
            }
        }
    }
    if (padding_zero_count)
    {
        for (int i = 0; i < padding_zero_count; i++)
        {
            *(low + (flags & LEFT_ALIGN ? 0 : padding_blank_count) + i) = '0';
        }
    }
}

int atoi(char **pstr)
{
    while (**pstr == ' ')
    {
        *(*pstr)++;
    };

    int num = 0;
    int neg;
    if (**pstr == '-')
    {
        neg = 1;
        *pstr++;
    }
    else if (**pstr == '+')
    {
        neg = 0;
        *pstr++;
    }
    else
    {
        neg = 0;
    }

    while (**pstr >= '0' && **pstr <= '9')
    {
        num = num * 10 + *(*pstr)++ - '0';
    }
    if (neg)
    {
        num *= -1;
    }
    return num;
}

void parse_format(char **pstr, const char **pformat, va_list arg, char *low)
{
    enum FORMAT_STATE state = INTRODUCE;
    enum FORMAT_FLAGS flags = NONE;
    enum FORMAT_SIZE size = -1;
    unsigned int width = -1;
    unsigned int precision = -1;

    do
    {
        if (state <= FLAG)
        {
            switch (**pformat)
            {
            case '#':
                flags |= ALTERNATE;
                state = FLAG;
                continue;
            case '0':
                flags |= ZERO_PADDED;
                state = FLAG;
                continue;
            case '-':
                flags |= LEFT_ALIGN;
                state = FLAG;
                continue;
            case ' ':
                flags |= PREFIX_BLANK;
                state = FLAG;
                continue;
            case '+':
                flags |= PREFIX_SIGN;
                state = FLAG;
                continue;
            default:
                state = WIDTH;
                break;
            }
        }
        if (state <= WIDTH)
        {
            if (**pformat == '*')
            {
                width = va_arg(arg, int);
            }
            else
            {
                width = atoi(pformat);
            }
            state = WIDTH;
        }
        if (state < PRECISION)
        {
            if (**pformat == '.')
            {
                state = PRECISION;
                continue;
            }
        }
        if (state == PRECISION)
        {
            if (**pformat == '*')
            {
                precision = va_arg(arg, int);
            }
            else
            {
                precision = atoi(pformat);
            }
            state = SIZE;
        }
        if (state <= SIZE)
        {
            switch (**pformat)
            {
            case 'h':
                if (*(*pformat + 1) == 'h')
                {
                    size = CHAR;
                }
                else
                {
                    size = SHORT;
                }
                state = SPECIFIER;
                continue;
            case 'l':
                if (*(*pformat + 1) == 'l')
                {
                    size = LONG_LONG;
                    *(*pformat)++;
                }
                else
                {
                    size = LONG;
                }
                state = SPECIFIER;
                continue;
            case 'L':
                size = LONG_DOUBLE;
                state = SPECIFIER;
                continue;
            case 'J':
                size = INTMAX;
                state = SPECIFIER;
                continue;
            case 'z':
                size = SIZE_T;
                state = SPECIFIER;
                continue;
            case 't':
                size = PTRDIFF_T;
                state = SPECIFIER;
                continue;
            default:
                state = SPECIFIER;
                break;
            }
        }
        if (state <= SPECIFIER)
        {
            switch (**pformat)
            {
            case '%':
                *pstr++;
                *(*pstr)++ = '%';
                return;
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                switch (size)
                {
                // Due to the default type promotion of C, char and short will promote to int.
                case CHAR:
                    char hhdata = va_arg(arg, int);
                    print_int(hhdata, pstr, **pformat, flags, width, precision);
                    return;
                case SHORT:
                    short hdata = va_arg(arg, int);
                    print_int(hdata, pstr, **pformat, flags, width, precision);
                    return;
                case LONG:
                    long ldata = va_arg(arg, long);
                    print_int(ldata, pstr, **pformat, flags, width, precision);
                    return;
                case LONG_LONG:
                    long long lldata = va_arg(arg, long long);
                    print_int(lldata, pstr, **pformat, flags, width, precision);
                    return;
                default:
                    int idata = va_arg(arg, int);
                    print_int(idata, pstr, **pformat, flags, width, precision);
                    return;
                }
            case 'c':
                char c = va_arg(arg, int);
                *(*pstr)++ = c;
                return;
            case 's':
                char *s = va_arg(arg, char *);
                while (*s)
                {
                    *(*pstr)++ = *s++;
                }
                return;
            case 'p':
                void *ptr = va_arg(arg, void*);
                // %p outputs as %#x or %#lx
                print_int(ptr, pstr, 'x', flags | ALTERNATE, width, precision);
                return;
            case 'n':
                switch (size)
                {
                // Due to the default type promotion of C, char and short will promote to int.
                case CHAR:
                    char *hhptr = va_arg(arg, int*);
                    *hhptr = *pstr - low + 1;
                    return;
                case SHORT:
                    short *hptr = va_arg(arg, int*);
                    *hptr = *pstr - low + 1;
                    return;
                case LONG:
                    long *lptr = va_arg(arg, long*);
                    *lptr = *pstr - low + 1;
                    return;
                case LONG_LONG:
                    long long *llptr = va_arg(arg, long long*);
                    *llptr = *pstr - low + 1;
                    return;
                default:
                    int *iptr = va_arg(arg, int*);
                    *iptr = *pstr - low + 1;
                    return;
                }
            default:
                break;
            }
        }
    } while (*(*pformat)++);
}

int vskprintf(char *str, const char *format, va_list arg)
{
    char *low = str;
    do
    {
        if (*format == '%')
        {
            format++;
            parse_format(&str, &format, arg, low);
        }
        else
        {
            *str++ = *format;
        }

    } while (*format++);
    *str = '\0';
    return str - low;
}

int skprintf(char *str, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    int ret = vskprintf(str, format, arg);
    va_end(arg);
    return ret;
}

int vkprintf(const char *format, va_list arg)
{
    char buffer[1024];
    int ret = vskprintf(buffer, format, arg);
    terminal_puts(buffer);
    return ret;
}

int kprintf(const char *format, ...)
{
    va_list arg;
    va_start(arg, *format);
    int ret = vkprintf(format, arg);
    va_end(arg);
    return ret;
}