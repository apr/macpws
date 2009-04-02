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

#include <algorithm>
#include <cryptopp/hmac.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/twofish.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <unistd.h>

#include "dbiov3.h"
#include "db.h"
#include "exception.h"
#include "keystretch.h"
#include "platform.h"
#include "util.h"


namespace pws {

namespace {

const int BLOCK_SIZE = CryptoPP::Twofish::BLOCKSIZE;

// This is an exception thrown by a low level reading method
// when an enf of file block is encountered.
struct end_of_file {
    static const char eof_tag[];
};

const char end_of_file::eof_tag[] = "PWS3-EOFPWS3-EOF";

const char pws_tag[] = {'P', 'W', 'S', '3'};

// Default number of iterations used to calculate the stretched
// key when writing the file.
const int keystretch_iter = 2048;


// The reader should be discarded after calling the read() method.
class reader {
public:
    reader(FILE *file, const std::string &key);

    pws_db *read();

private:
    reader(const reader &);
    reader &operator= (const reader &);

    void read_file(void *buf, int len);

    // Reads a block (of the cipher block size) into the buffer
    // and decrypts it. Returns false if the block was the EOF block.
    void read_cbc(void *buf);

    void check_tag();
    void check_passphrase();
    void check_hmac();
    void read_b_fields();
    void read_fields(field_holder &fields);
    void read_header(pws_db &db);
    void read_records(pws_db &db);

    // Reads one fields and returns the type of the field. The data buffer
    // passed to the method will be filled in with the field's data.
    int read_field(std::string &data);

private:
    FILE *_file;
    std::string _key;
    std::string _stretched_key;

    CryptoPP::CBC_Mode<CryptoPP::Twofish>::Decryption _cipher;
    CryptoPP::HMAC<CryptoPP::SHA256> _hmac;

    byte _k[BLOCK_SIZE * 2];
    byte _l[BLOCK_SIZE * 2];
    byte _iv[BLOCK_SIZE];
};


// The writer should be discarded after calling the write() method.
class writer {
public:
    writer(FILE *file, const pws_db &db, const std::string &key);

    void write();

private:
    writer(const writer &);
    writer &operator= (const writer &);

    void write_file(const void *buf, int len);

    // Writes a block (of the cipher block size) from the buffer to the file
    // encrypting it.
    void write_cbc(const void *buf);

    void write_tag();
    void write_passphrase();
    void write_b_fields();
    void write_iv();
    void write_field(int type, const std::string &data);
    void write_fields(const field_holder &fields);
    void write_records();
    void write_eof();
    void write_hmac();

private:
    FILE *_file;
    const pws_db &_db;
    std::string _key;
    std::string _stretched_key;

    CryptoPP::AutoSeededRandomPool _rng;
    CryptoPP::CBC_Mode<CryptoPP::Twofish>::Encryption _cipher;
    CryptoPP::HMAC<CryptoPP::SHA256> _hmac;

    byte _k[BLOCK_SIZE * 2];
    byte _l[BLOCK_SIZE * 2];
    byte _iv[BLOCK_SIZE];
};


reader::reader(FILE *file, const std::string &key)
    : _file(file), _key(key)
{
}

void reader::read_file(void *buf, int len)
{
    if(fread(buf, 1, len, _file) != len) {
        throw pws_io_exception(MALFORMED_FILE);
    }
}

void reader::read_cbc(void *buf)
{
    read_file(buf, BLOCK_SIZE);

    if (memcmp(buf, end_of_file::eof_tag, BLOCK_SIZE) == 0) {
        throw end_of_file();
    }

    _cipher.ProcessData((byte *)buf, (const byte *)buf, BLOCK_SIZE);
}

void reader::check_tag() {
    char buf[4];

    if(fread(buf, 1, sizeof(buf), _file) != sizeof(buf)) {
        throw pws_io_exception(INVALID_TAG);
    }

    if (memcmp(pws_tag, buf, sizeof(buf)) != 0) {
        throw pws_io_exception(INVALID_TAG);
    }
}

void reader::check_passphrase()
{
    char salt[32];
    unsigned char n_iter[4];

    read_file(salt, sizeof(salt));
    read_file(n_iter, sizeof(n_iter));

    _stretched_key = stretch_key(std::string(salt, sizeof(salt)), _key,
        get_int32le(n_iter));

    CryptoPP::SHA256 h;
    const int digestsize = CryptoPP::SHA256::DIGESTSIZE;
    byte key_hash[digestsize];
    byte saved_key_hash[digestsize];

    read_file(saved_key_hash, sizeof(saved_key_hash));

    h.Update((const byte *)_stretched_key.c_str(), _stretched_key.length());
    h.Final(key_hash);

    if (memcmp(key_hash, saved_key_hash, digestsize) != 0) {
        throw pws_io_exception(INVALID_PASSWORD);
    }
}

void reader::check_hmac()
{
    char buf[_hmac.DIGESTSIZE];
    byte hmac_out[_hmac.DIGESTSIZE];

    _hmac.Final(hmac_out);
    read_file(buf, sizeof(buf));

    if(memcmp(buf, hmac_out, sizeof(buf)) != 0) {
        throw pws_io_exception(HMAC_DID_NOT_MATCH);
    }
}

void reader::read_b_fields()
{
    read_file(_k, sizeof(_k));
    read_file(_l, sizeof(_l));

    CryptoPP::ECB_Mode<CryptoPP::Twofish>::Decryption twofish;

    twofish.SetKey((const byte *)_stretched_key.c_str(),
        _stretched_key.length());

    twofish.ProcessData(_k, _k, sizeof(_k));
    twofish.ProcessData(_l, _l, sizeof(_l));
}

int reader::read_field(std::string &data)
{
    byte buf[BLOCK_SIZE];

    read_cbc(buf);

    int type = buf[4];
    int to_read = get_int32le(buf);
    int data_len = std::min(to_read, BLOCK_SIZE - 5);

    data.clear();
    data.append((char *)buf + 5, data_len);
    _hmac.Update((byte *)buf + 5, data_len);
    to_read -= data_len;

    while(to_read > 0) {
        read_cbc(buf);
        data_len = std::min(to_read, BLOCK_SIZE);
        data.append((char *)buf, data_len);
        _hmac.Update((byte *)buf, data_len);
        to_read -= data_len;
    }

    return type;
}

void reader::read_fields(field_holder &fields)
{
    std::string data;
    int type;

    do {
        type = read_field(data);
        if(type != 0xff) {
            fields.add_raw_field(type, data);
        }
    } while(type != 0xff);
}

void reader::read_header(pws_db &db)
{
    try {
        read_fields(db.get_header().get_fields());
    } catch(end_of_file ex) {
        throw pws_io_exception(MALFORMED_FILE);
    }
}

void reader::read_records(pws_db &db)
{
    while(1) {
        try {
            scoped_ptr<pws_record> rec(db.create_empty_record());
            read_fields(rec->get_fields());
            db.add_record(rec.release());
        } catch(end_of_file ex) {
            break;
        }
    }
}

pws_db *reader::read()
{
    scoped_ptr<pws_db> db(pws_db::create_empty());

    check_tag();
    check_passphrase();
    read_b_fields();
    read_file(_iv, sizeof(_iv));

    _cipher.SetKeyWithIV(_k, sizeof(_k), _iv);
    _hmac.SetKey(_l, sizeof(_l));

    read_header(*db);
    read_records(*db);

    check_hmac();

    return db.release();
}

writer::writer(FILE *file, const pws_db &db, const std::string &key)
    : _file(file), _db(db), _key(key)
{
}

void writer::write_file(const void *buf, int len)
{
    if(fwrite(buf, 1, len, _file) != len) {
        throw pws_io_exception(WRITE_ERROR);
    }
}

void writer::write_cbc(const void *buf)
{
    byte out[BLOCK_SIZE];
    _cipher.ProcessData(out, (const byte *)buf, sizeof(out));
    write_file(out, sizeof(out));
}

void writer::write_tag()
{
    write_file(pws_tag, sizeof(pws_tag));
}

void writer::write_passphrase()
{
    char salt[32];
    unsigned char n_iter[4];

    _rng.GenerateBlock((byte *)salt, sizeof(salt));
    put_int32le(keystretch_iter, n_iter);

    _stretched_key = stretch_key(std::string(salt, sizeof(salt)),
        _key, keystretch_iter);

    CryptoPP::SHA256 h;
    const int digestsize = CryptoPP::SHA256::DIGESTSIZE;
    byte key_hash[digestsize];

    h.Update((const byte *)_stretched_key.c_str(), _stretched_key.length());
    h.Final(key_hash);

    write_file(salt, sizeof(salt));
    write_file(n_iter, sizeof(n_iter));
    write_file(key_hash, sizeof(key_hash));
}

void writer::write_b_fields()
{
    CryptoPP::ECB_Mode<CryptoPP::Twofish>::Encryption twofish;

    twofish.SetKey((const byte *)_stretched_key.c_str(),
        _stretched_key.length());

    _rng.GenerateBlock(_k, sizeof(_k));
    _rng.GenerateBlock(_l, sizeof(_l));

    byte buf_k[sizeof(_k)];
    byte buf_l[sizeof(_l)];

    twofish.ProcessData(buf_k, _k, sizeof(_k));
    twofish.ProcessData(buf_l, _l, sizeof(_l));

    write_file(buf_k, sizeof(buf_k));
    write_file(buf_l, sizeof(buf_l));
}

void writer::write_iv()
{
    _rng.GenerateBlock(_iv, sizeof(_iv));
    write_file(_iv, sizeof(_iv));
}

void writer::write_field(int type, const std::string &data)
{
    byte buf[BLOCK_SIZE];
    int to_write = data.size();

    put_int32le(data.size(), buf);
    buf[4] = type;

    int buf_off = 5; // account for the len and type in the first block
    int data_off = 0;

    do {
        int copy_len = std::min(to_write, BLOCK_SIZE - buf_off);

        memcpy(buf + buf_off, data.c_str() + data_off, copy_len);

        // If there is some unfilled space at the end of the buffer,
        // pad it with random data.
        if((buf_off + copy_len) < sizeof(buf)) {
            _rng.GenerateBlock(buf + buf_off + copy_len,
                BLOCK_SIZE - buf_off - copy_len);
        }

        write_cbc(buf);
        _hmac.Update(buf + buf_off, copy_len);

        buf_off = 0;

        data_off += copy_len;
        to_write -= copy_len;
    } while(to_write > 0);
}

void writer::write_fields(const field_holder &fields)
{
    if(fields.num_fields() == 0) {
        return;
    }

    for(int i = 0; i < fields.num_fields(); ++i) {
        const pws_field &f = fields.get_field_by_index(i);
        write_field(f.get_type(), f.get_data());
    }

    write_field(0xff, std::string());
}

void writer::write_records()
{
    for(int i = 0; i < _db.num_records(); ++i) {
        const pws_record &r = _db.get_record_by_index(i);
        write_fields(r.get_fields());
    }
}

void writer::write_eof()
{
    // write the EOF block
    write_file(end_of_file::eof_tag, BLOCK_SIZE);
}

void writer::write_hmac()
{
    byte buf[_hmac.DIGESTSIZE];
    _hmac.Final(buf);
    write_file(buf, sizeof(buf));
}

void writer::write()
{
    write_tag();
    write_passphrase();
    write_b_fields();
    write_iv();

    _cipher.SetKeyWithIV(_k, sizeof(_k), _iv);
    _hmac.SetKey(_l, sizeof(_l));

    write_fields(_db.get_header().get_fields());
    write_records();

    write_eof();
    write_hmac();
}

} // namespace


db_reader_v3::db_reader_v3(const std::string &file, const std::string &key)
    : _file(file), _key(key)
{
}

pws_db *db_reader_v3::read()
{
    file_guard f(fopen(_file.c_str(), "rb"));

    if(f.file() == 0) {
        throw pws_io_exception(FILE_NOT_FOUND);
    }

    reader r(f.file(), _key);
    return r.read();
}


namespace {

// A helper class that generates a unique temporary filename
// alongside the given file and using the given file name as
// a template. This class ensures that the temporary file is
// removed when it is going out of scope.
class tmp_file {
public:
    explicit tmp_file(const std::string &orig_name)
    {
        std::string dir(dirname(orig_name));

        if(dir.empty()) {
            dir = "./";
        }

        char *tm = tempnam(dir.c_str(), filename(orig_name + ".").c_str());
        _tmp_name = tm;
        free(tm);
    }

    ~tmp_file()
    {
        unlink(_tmp_name.c_str());
    }

    const std::string &name() const { return _tmp_name; }

private:
    tmp_file(const tmp_file &);
    tmp_file &operator= (const tmp_file &);

    std::string _tmp_name;
};

} // namespace


void db_writer_v3::write(pws_db &db, const std::string &file,
    const std::string &key)
{
    tmp_file temp(file);

    // Create a scope to do a write in so that the temporary file is
    // closed before the move.
    {
        file_guard f(fopen(temp.name().c_str(), "wb"));

        if(f.file() == 0) {
            throw pws_io_exception(CANNOT_WRITE_FILE);
        }

        // TODO update the db with the user and host

        writer w(f.file(), db, key);
        w.write();
    }

    // Now that the write's been successful we can move the temporary file
    // in place of the actual database.
    if(rename(temp.name().c_str(), file.c_str()) != 0) {
        throw pws_io_exception();
    }
}

}

