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

#ifndef _PWS_DB_H_
#define _PWS_DB_H_

#include <string>
#include <vector>
#include <uuid/uuid.h>


namespace pws {

// Immutable class that represents a field in the pws database.
class pws_field {
public:
    pws_field(int type, const std::string &data);

    int get_type() const;
    const std::string &get_data() const;

    const std::string &get_text() const { return get_data(); }
    unsigned int get_time() const { return get_int32(); }
    unsigned int get_int16() const;
    unsigned int get_int32() const;
    void get_uuid(uuid_t out) const;

private:
    pws_field(const pws_field &);
    pws_field &operator= (const pws_field &);

    int _type;
    std::string _data;
};


// A collection of fields.
class field_holder {
public:
    field_holder() {}
    ~field_holder();

    void add_raw_field(int type, const std::string &data);
    void add_int16_field(int type, int data);
    void add_uuid_field(int type, uuid_t data);

    // Replaces the previous field of the same type, it is assumed
    // that there is only one occurence of the field. The effect of this
    // method is first calling remove_field() on the given type and then
    // calling add_raw_field() with the new data with the exception that
    // the order of fields is preserved.
    void set_field(int type, const std::string &data);

    // Will remove all fields of the given type.
    void remove_field(int type);

    bool has_field(int type) const;

    // Will return a first occurence of the field of the given type.
    pws_field &get_field_by_type(int type);
    pws_field &get_field_by_index(int index);
    const pws_field &get_field_by_type(int type) const;
    const pws_field &get_field_by_index(int index) const;

    // Total number of fields in the store.
    int num_fields() const;
    
private:
    field_holder(const field_holder &);
    field_holder &operator= (const field_holder &);

    std::vector<pws_field *> _fields;
};

class pws_header {
public:
    enum field_type_t {
        VERSION = 0x00,
        UUID = 0x01,
        PREFERENCES = 0x02,
        TREE_DISPLAY_STATUS = 0x03,
        TIME_LAST_SAVE = 0x04,
        WHO_SAVED = 0x05,
        WHAT_SAVED = 0x06,
        LAST_USER = 0x07,
        LAST_HOST = 0x08,
        DB_NAME = 0x09,
        DB_DESCRIPTION = 0x0a,
        DB_FILTERS = 0x0b
    };

    pws_header() {}

    // TODO add setters and getters for different fields
    // TODO move to the db??
    int get_version() const;

    field_holder &get_fields() { return _fields; }
    const field_holder &get_fields() const { return _fields; }

    // This method is considered low-level and is used by the
    // readers to populate an empty header.
    void add_raw_field(field_type_t type, const std::string &data);

private:
    pws_header(const pws_header &);
    pws_header &operator= (const pws_header &);

    field_holder _fields;
};


class pws_record {
public:
    enum field_type_t {
        UUID = 0x01,
        GROUP = 0x02,
        TITLE = 0x03,
        USERNAME = 0x04,
        NOTES = 0x05,
        PASSWORD = 0x06,
        CREATION_TIME = 0x07,
        PASS_MODIFICATION_TIME = 0x08,
        LAST_ACCESS_TIME = 0x09,
        PASS_EXPIRY_TIME = 0x0a,
        LAST_MODIFICATION_TIME = 0x0c,
        URL = 0x0d,
        AUTOTYPE = 0x0e,
        PASS_HISTORY = 0x0f,
        PASS_POLICY = 0x10,
        PASS_EXPIRY_INTERVAL = 0x11
    };

    pws_record(const std::string &title, const std::string &pass);

    std::string get_group() const;
    std::string get_title() const;
    std::string get_username() const;
    std::string get_password() const;
    std::string get_notes() const;

    void set_group(const std::string &);
    void set_title(const std::string &);
    void set_username(const std::string &);
    void set_password(const std::string &);
    void set_notes(const std::string &);

    field_holder &get_fields() { return _fields; }
    const field_holder &get_fields() const { return _fields; }

    // This method is considered low-level and is used by the readers
    // to populate an empty record.
    void add_raw_field(field_type_t type, const std::string &data);

private:
    pws_record() {}

    pws_record(const pws_record &);
    pws_record &operator= (const pws_record &);

    field_holder _fields;

    friend class pws_db;
};


class pws_db {
public:
    // Creates an empty database with the given version and generates
    // a new UUID.
    explicit pws_db(int version);

    ~pws_db();

    // Creates empty database to be filled from the file. A regular
    // application should not use this call because some of the required
    // fields might be missing.
    // The caller assumes ownership of the pointer.
    static pws_db *create_empty();

    pws_header &get_header();
    const pws_header &get_header() const;

    // Adds a new record with the given title and password to the database
    // and returns a reference to it.
    // Generates UUID for the record.
    pws_record &add_record(const std::string &title, const std::string &pass);

    int num_records() const;

    pws_record &get_record_by_index(int index);
    const pws_record &get_record_by_index(int index) const;

    void delete_record(const pws_record &);
    void delete_record_by_index(int index);

    // Creates a new empty record and returns a reference to it.
    // An application should not use this call directly because it does not
    // populate required fields.
    pws_record &add_empty_record();

private:
    pws_db() {}

    pws_db(const pws_db &);
    pws_db &operator= (const pws_db &);

    pws_header _header;
    std::vector<pws_record *> _records;
};

}

#endif

