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

#include <errno.h>
#include "exception.h"

namespace pws {

namespace {

char *error_table[] = {
    "Invalid file format tag, not a PasswordSafe V3 database",
    "Malformed database",
    "Invalid password",
    "File not found",
    "File authentication code (HMAC) did not match",
    "Cannot open file for write",
    "Cannot write to file",
};

} // namespace

pws_io_exception::pws_io_exception()
    : _error_code(UNSPECIFIED), _errno(errno)
{
}

pws_io_exception::pws_io_exception(io_error_code_t error_code)
    : _error_code(error_code), _errno(errno)
{
}

io_error_code_t pws_io_exception::error_code() const
{
    return _error_code;
}

const char *pws_io_exception::what() const throw()
{
    const int error_table_len = sizeof(error_table) / sizeof(error_table[0]);
    if (_error_code >= error_table_len) {
        return "Unknown";
    }

    return error_table[_error_code];
}

const char *field_not_found::what() const throw()
{
    return "field not found";
}

}

