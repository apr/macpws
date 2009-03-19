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

#ifndef _PWS_DB_UTIL_H_
#define _PWS_DB_UTIL_H_

#include <stdio.h>
#include <string>

namespace pws {

// Extracts the directory name from the given path. If the file in
// the path is in the current directory returns an empty string.
std::string dirname(const std::string &path);

// Returns the file name after the last '/' from the given path.
std::string filename(const std::string &path);


// A helper class that ensures that a file stream is closed
// when going out of scope.
class file_guard {
public:
    explicit file_guard(FILE *file) : _file(file) {}
    ~file_guard() { fclose(_file); }

    FILE *file() { return _file; }

private:
    file_guard(const file_guard &);
    file_guard &operator= (const file_guard &);

    FILE *_file;
};


// A very simple smart pointer that deletes the pointer it owns
// when going out of scope.
template<class T>
class scoped_ptr {
public:
    explicit scoped_ptr(T *ptr) : _ptr(ptr) {}
    ~scoped_ptr() { delete _ptr; }

    T &operator* () { return *_ptr; }
    const T &operator* () const { return *_ptr; }
    T *operator-> () { return _ptr; }
    const T *operator-> () const { return _ptr; }

    T *release() {
        T *ret = _ptr;
        _ptr = 0;
        return ret;
    }

    void reset(T *ptr) {
        delete _ptr;
        _ptr = ptr;
    }

private:
    scoped_ptr(const scoped_ptr &);
    scoped_ptr &operator= (const scoped_ptr &);

    T *_ptr;
};

}

#endif

