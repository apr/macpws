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

#import "Record.h"

#import "db/exception.h"


@implementation Record

- (id) initWithRecord: (pws::pws_record *)r
{
    if(!(self = [super init])) {
        return nil;
    }
    
    record = r;
    
    return self;
}

- (NSString *) name
{
    NSString *ret = [self title];
    
    if([ret length] == 0) {
        return @"<empty>";
    }

    return ret;
}

- (NSString *) fullGroupName
{
    return [NSString stringWithUTF8String: record->get_group().c_str()];
}

- (NSString *) title
{
    return [NSString stringWithUTF8String: record->get_title().c_str()];
}

- (NSString *) username
{
    return [NSString stringWithUTF8String: record->get_username().c_str()];
}

- (NSString *) password
{
    return [NSString stringWithUTF8String: record->get_password().c_str()];
}

- (NSString *) notes
{
    return [NSString stringWithUTF8String: record->get_notes().c_str()];
}

- (void) setTitle: (NSString *)title
{
    record->set_title([title UTF8String]);
}

- (void) setUsername: (NSString *)username
{
    record->set_username([username UTF8String]);
}

- (void) setPassword: (NSString *)password
{
    record->set_password([password UTF8String]);
}

- (void) setNotes: (NSString *)notes
{
    record->set_notes([notes UTF8String]);
}

- (pws::pws_record *) dbRecord
{
    return record;
}

- (void) setGroup: (Group *)g
{
    group = g;
}

- (Group *) group
{
    return group;
}

@end
