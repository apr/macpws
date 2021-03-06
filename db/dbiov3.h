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

#ifndef _PWS_DB_IO_V3_H_
#define _PWS_DB_IO_V3_H_

#include "db_reader.h"
#include "db_writer.h"

// IO operations for pws database v3.

namespace pws {

class db_reader_v3 : public db_reader {
public:
    db_reader_v3(const std::string &file, const std::string &key);

    virtual pws_db *read();

private:
    db_reader_v3(const db_reader_v3 &);
    db_reader_v3 &operator= (const db_reader_v3 &);

    std::string _file;
    std::string _key;
};


class db_writer_v3 : public db_writer {
public:
    db_writer_v3() {};

    virtual void write(pws_db &db, const std::string &file,
        const std::string &key);

private:
    db_writer_v3(const db_writer_v3 &);
    db_writer_v3 &operator= (const db_writer_v3 &);
};

}

#endif

