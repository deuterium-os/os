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
    TYPE,
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

    int digitnum = 0; // The number of digits, don't include prefix
    int value_tmp = value;
    do
    {
        value_tmp /= radix;
        digitnum++;
    } while (value_tmp);

    int len = digitnum; // The length of string, includes prefix
    if (type == 'd' || type == 'u')
    {
        if (value < 0)
        {
            *(*pstr)++ = '-';
            len++;
        }
        else if (flags & PREFIX_SIGN)
        {
            *(*pstr)++ = '+';
            len++;
        }
        else if (flags & PREFIX_BLANK)
        {
            *(*pstr)++ = ' ';
            len++;
        }
    }

    if (width > len && !(flags & LEFT_ALIGN))
    {
        int count = width - len;
        for (int i = 0; i < count; i++)
        {
            *(*pstr)++ = flags & ZERO_PADDED ? '0' : ' ';
        }
    }

    while (precision > digitnum)
    {
        *(*pstr)++ = '0';
        digitnum++;
    }

    char *low, *org;
    low = org = *pstr;
    do
    {
        *(*pstr)++ = (type == 'X' ? "FEDCBA9876543210123456789ABCDEF" : "fedcba9876543210123456789abcdef")[15 + value % radix];
        value /= radix;
        digitnum++;
    } while (value);

    char *high = *pstr - 1;
    while (low < high)
    {
        char tmp = *low;
        *low++ = *high;
        *high-- = tmp;
    }

    if (width > len && flags & LEFT_ALIGN)
    {
        int count = width - len;
        for (int i = 0; i < count; i++)
        {
            *(*pstr)++ = ' ';
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

void parse_format(char **pstr, const char **pformat, va_list arg)
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
                state = TYPE;
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
                state = TYPE;
                continue;
            case 'L':
                size = LONG_DOUBLE;
                state = TYPE;
                continue;
            case 'J':
                size = INTMAX;
                state = TYPE;
                continue;
            case 'z':
                size = SIZE_T;
                state = TYPE;
                continue;
            case 't':
                size = PTRDIFF_T;
                state = TYPE;
                continue;
            default:
                state = TYPE;
                break;
            }
        }
        if (state <= TYPE)
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
                    short int hdata = va_arg(arg, int);
                    print_int(hdata, pstr, **pformat, flags, width, precision);
                    return;
                case LONG:
                    long int ldata = va_arg(arg, long);
                    print_int(ldata, pstr, **pformat, flags, width, precision);
                    return;
                case LONG_LONG:
                    long long int lldata = va_arg(arg, long long);
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
            parse_format(&str, &format, arg);
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