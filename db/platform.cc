/*
 * Copyright (c) 2009 Alex Raschepkin
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "platform.h"

bool pws::is_platform_le()
{
    int n = 0x01;
    return *((char *)&n) == 1;
}

unsigned int pws::get_int32le(const unsigned char *buf)
{
    unsigned int ret = 0;

    ret |= (unsigned int)buf[3] << 24;
    ret |= (unsigned int)buf[2] << 16;
    ret |= (unsigned int)buf[1] << 8;
    ret |= (unsigned int)buf[0];

    return ret;
}

unsigned int pws::get_int16le(const unsigned char *buf)
{
    unsigned int ret = 0;

    ret |= (unsigned int)buf[1] << 8;
    ret |= (unsigned int)buf[0];

    return ret;
}

void pws::put_int32le(unsigned int n, unsigned char *buf)
{
    buf[0] = (unsigned char)(n & 0xff);
    buf[1] = (unsigned char)((n >> 8) & 0xff);
    buf[2] = (unsigned char)((n >> 16) & 0xff);
    buf[3] = (unsigned char)((n >> 24) & 0xff);
}

void pws::put_int16le(unsigned int n, unsigned char *buf)
{
    buf[0] = (unsigned char)(n & 0xff);
    buf[1] = (unsigned char)((n >> 8) & 0xff);
}

