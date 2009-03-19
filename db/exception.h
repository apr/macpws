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

#ifndef _PWD_DB_EXCEPTION_H_
#define _PWD_DB_EXCEPTION_H_

#include <exception>

namespace pws {

class field_not_found : public std::exception {
public:
    virtual const char *what() const throw();
};

enum io_error_code_t {
    INVALID_TAG,
    MALFORMED_FILE,
    INVALID_PASSWORD,
    FILE_NOT_FOUND,
    HMAC_DID_NOT_MATCH,
    CANNOT_WRITE_FILE,
    WRITE_ERROR,
    UNSPECIFIED,
};

class pws_io_exception : public std::exception {
public:
    pws_io_exception();
    explicit pws_io_exception(io_error_code_t error_code);

    io_error_code_t error_code() const;

    virtual const char *what() const throw();

private:
    io_error_code_t _error_code;
    int _errno;
};

}

#endif

