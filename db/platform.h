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

#ifndef _PWS_DB_PLATFORM_H_
#define _PWS_DB_PLATFORM_H_

namespace pws {

// Returns true if executed in a little endian environment
bool is_platform_le();

// Reads and returns an int32 stored in little endian
// format in the given buffer.
unsigned int get_int32le(const unsigned char *buf);

// Reads and returns an int16 stored in little endian
// format in the given buffer.
unsigned int get_int16le(const unsigned char *buf);

// Storea the given int32 number in the given buffer
// in little endian format.
void put_int32le(unsigned int n, unsigned char *buf);

// Stores the given int16 number in the given buffer
// in little endian format.
void put_int16le(unsigned int n, unsigned char *buf);

}

#endif

