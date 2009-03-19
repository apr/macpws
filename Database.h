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

#import "db/db.h"
#import "Group.h"


#define PwsErrorDomain @"MacSafeErrorDomain"


@interface Database : NSObject {
    pws::pws_db *db;
    NSMutableArray *objects;
    NSString *filename;
    NSString *key;
}

// Returns nil in case of an error.
+ (Database *) createEmpty: (NSString *)path withKey: (NSString *)password
                    error: (NSError **)outError;

// Returns nil in case of an error.
+ (Database *) readFromFile: (NSString *)path withKey: (NSString *)password
                    error: (NSError **)outError;

- (id) initWithPath: (NSString *)path
       key: (NSString *)k
       database: (pws::pws_db *)database;
- (void) dealloc;

// Save methods return NO if there was an error.
- (BOOL) save: (NSError **)outError;
- (BOOL) saveWithNewKey: (NSString *)password error: (NSError **)outError;

- (NSString *) filename;

- (Record *) createNewRecordWithTitle: (NSString *)title inGroup: (Group *)group;
- (Group *) createNewGroupWithName: (NSString *)name parentGroup: (Group *)group;

// After this call the record is no longer valid and should be discarded.
- (void) deleteRecord: (Record *)record;

// After this call the group is no longer valid and should be discarded.
// Note: deleting a group will delete all the records belonging to this group.
- (void) deleteGroup: (Group *)group;

- (NSArray *) rootObjects;

@end
