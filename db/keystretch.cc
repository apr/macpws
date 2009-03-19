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

#include "keystretch.h"
#include <cryptopp/sha.h>


std::string pws::stretch_key(const std::string &salt,
    const std::string &key, int n_iter)
{
    const int digestsize = CryptoPP::SHA256::DIGESTSIZE;

    CryptoPP::SHA256 h0;
    byte buf[digestsize];

    h0.Update((const byte *)key.c_str(), key.length());
    h0.Update((const byte *)salt.c_str(), salt.length());
    h0.Final(buf);

    for (int i = 0; i < n_iter; ++i) {
        CryptoPP::SHA256 h;

        h.Update(buf, digestsize);
        h.Final(buf);
    }

    return std::string((char *)buf, sizeof(buf));
}

