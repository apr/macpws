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

#include <assert.h>
#include <functional>
#include <string.h>

#include "db.h"
#include "exception.h"
#include "platform.h"


pws::pws_field::pws_field(int type, const std::string &data)
    : _type(type), _data(data)
{
}

int pws::pws_field::get_type() const
{
    return _type;
}

const std::string &pws::pws_field::get_data() const
{
    return _data;
}

unsigned int pws::pws_field::get_int16() const
{
    assert(_data.size() >= 2);
    return get_int16le((const unsigned char *)_data.c_str());
}

unsigned int pws::pws_field::get_int32() const
{
    assert(_data.size() >= 4);
    return get_int32le((const unsigned char *)_data.c_str());
}

void pws::pws_field::get_uuid(uuid_t out) const
{
    assert(_data.size() == 16);
    memcpy(out, _data.c_str(), sizeof(uuid_t));
}


pws::field_holder::~field_holder()
{
    for(int i = 0; i < _fields.size(); ++i) {
        delete _fields[i];
    }
}

void pws::field_holder::add_raw_field(int type, const std::string &data)
{
    _fields.push_back(new pws_field(type, data));
}

void pws::field_holder::add_int16_field(int type, int data)
{
    unsigned char buf[2];
    put_int16le(data, buf);
    _fields.push_back(
        new pws_field(type, std::string((char *)buf, sizeof(buf))));
}

void pws::field_holder::add_uuid_field(int type, uuid_t data)
{
    _fields.push_back(new pws_field(type,
        std::string((char *)data, sizeof(uuid_t))));
}

void pws::field_holder::set_field(int type, const std::string &data)
{
    for(int i = 0; i < _fields.size(); ++i) {
        if(_fields[i]->get_type() == type) {
            delete _fields[i];
            _fields[i] = new pws_field(type, data);
            return;
        }
    }

    // No existing field, add new.
    add_raw_field(type, data);
}

bool pws::field_holder::has_field(int type) const
{
    for(int i = 0; i < _fields.size(); ++i) {
        if(_fields[i]->get_type() == type) {
            return true;
        }
    }

    return false;
}

namespace {

struct field_type_equal
        : public std::binary_function<int, pws::pws_field *, bool>
{
    bool operator() (int type, const pws::pws_field *field) const
    {
        return field->get_type() == type;
    }
};

} // namespace

void pws::field_holder::remove_field(int type)
{
    std::vector<pws_field *>::iterator i = std::remove_if(
        _fields.begin(), _fields.end(), std::bind1st(field_type_equal(), type));
    _fields.erase(i, _fields.end());
}

pws::pws_field &pws::field_holder::get_field_by_type(int type)
{
    for(int i = 0; i < _fields.size(); ++i) {
        if(_fields[i]->get_type() == type) {
            return *(_fields[i]);
        }
    }

    throw field_not_found();
}

const pws::pws_field &pws::field_holder::get_field_by_type(int type) const
{
    return const_cast<field_holder *>(this)->get_field_by_type(type);
}

pws::pws_field &pws::field_holder::get_field_by_index(int index)
{
    return *(_fields[index]);
}

const pws::pws_field &pws::field_holder::get_field_by_index(int index) const
{
    return *(_fields[index]);
}

int pws::field_holder::num_fields() const
{
    return _fields.size();
}


void pws::pws_header::add_raw_field(field_type_t type, const std::string &data)
{
    _fields.add_raw_field(type, data);
}

int pws::pws_header::get_version() const
{
    const pws_field &field = _fields.get_field_by_type(VERSION);
    return field.get_int16();
}


pws::pws_record::pws_record(const std::string &title, const std::string &pass)
{
    uuid_t uuid;

    uuid_generate(uuid);
    _fields.add_uuid_field(pws_record::UUID, uuid);
    _fields.add_raw_field(pws_record::TITLE, title);
    _fields.add_raw_field(pws_record::PASSWORD, pass);
}

std::string pws::pws_record::get_group() const
{
    try {
        return _fields.get_field_by_type(GROUP).get_text();
    } catch(field_not_found ex) {
        return "";
    }
}

std::string pws::pws_record::get_title() const
{
    try {
        return _fields.get_field_by_type(TITLE).get_text();
    } catch(field_not_found ex) {
        return "";
    }
}

std::string pws::pws_record::get_username() const
{
    try {
        return _fields.get_field_by_type(USERNAME).get_text();
    } catch(field_not_found ex) {
        return "";
    }
}

std::string pws::pws_record::get_password() const
{
    try {
        return _fields.get_field_by_type(PASSWORD).get_text();
    } catch(field_not_found ex) {
        return "";
    }
}

std::string pws::pws_record::get_notes() const
{
    try {
        return _fields.get_field_by_type(NOTES).get_text();
    } catch(field_not_found ex) {
        return "";
    }
}

void pws::pws_record::set_group(const std::string &g)
{
    _fields.set_field(GROUP, g);
}

void pws::pws_record::set_title(const std::string &t)
{
    _fields.set_field(TITLE, t);
}

void pws::pws_record::set_username(const std::string &u)
{
    _fields.set_field(USERNAME, u);
}

void pws::pws_record::set_password(const std::string &p)
{
    _fields.set_field(PASSWORD, p);
}

void pws::pws_record::set_notes(const std::string &n)
{
    _fields.set_field(NOTES, n);
}

void pws::pws_record::add_raw_field(field_type_t type, const std::string &data)
{
    _fields.add_raw_field(type, data);
}


pws::pws_db::pws_db(int version)
{
    uuid_t uuid;

    uuid_generate(uuid);
    _header.get_fields().add_int16_field(pws_header::VERSION, version);
    _header.get_fields().add_uuid_field(pws_header::UUID, uuid);
}

pws::pws_db *pws::pws_db::create_empty()
{
    return new pws_db;
}

pws::pws_header &pws::pws_db::get_header()
{
    return _header;
}

const pws::pws_header &pws::pws_db::get_header() const
{
    return _header;
}

pws::pws_db::~pws_db()
{
    for(int i = 0; i < _records.size(); ++i) {
        delete _records[i];
    }
}

pws::pws_record &pws::pws_db::add_record(
    const std::string &title, const std::string &pass)
{
    pws_record *ret = new pws_record(title, pass);
    _records.push_back(ret);
    return *ret;
}

pws::pws_record &pws::pws_db::add_empty_record()
{
    pws_record *ret = new pws_record();
    _records.push_back(ret);
    return *ret;
}

int pws::pws_db::num_records() const
{
    return _records.size();
}

pws::pws_record &pws::pws_db::get_record_by_index(int index)
{
    return *(_records[index]);
}

const pws::pws_record &pws::pws_db::get_record_by_index(int index) const
{
    return *(_records[index]);
}

void pws::pws_db::delete_record(const pws_record &r)
{
    for(int i = 0; i < _records.size(); ++i) {
        if(_records[i] == &r) {
            delete _records[i];
            break;
        }
    }

    std::vector<pws_record *>::iterator i = std::remove(
        _records.begin(), _records.end(), &r);
    _records.erase(i, _records.end());
}

void pws::pws_db::delete_record_by_index(int index)
{
    delete_record(*_records[index]);
}

