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

#import <Cocoa/Cocoa.h>

#import "Record.h"

@interface Group : NSObject {
    NSString *fullName;
    
    NSMutableArray *subgroups;
    NSMutableArray *records;
    
    // This reference should never be retained to avoid cycles.
    Group *parentGroup;
}

- (id) initWithName: (NSString *)name;
- (void) dealloc;

// Users are discouraged to use this method directly. The subgroups should
// be managed through their parent groups.
- (void) setParentGroup: (Group *)group;
- (Group *) parentGroup;

- (NSString *) name;

- (void) addRecord: (Record *)rec;
- (void) addGroup: (Group *)group;

- (void) removeRecord: (Record *)rec;
- (void) removeGroup: (Group *)group;

- (NSArray *) subgroups;
- (NSArray *) records;

// Enumerates all records belonging to this group and all its subgroups.
- (NSArray *) deepRecords;

@end
