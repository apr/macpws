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

#import "Group.h"


@implementation Group

- (id) initWithName: (NSString *)name
{
    if(!(self = [super init])) {
        return nil;
    }
    
    fullName = [name retain];
    subgroups = [[NSMutableArray alloc] init];
    records = [[NSMutableArray alloc] init];

    return self;
}

- (void) dealloc
{
    [fullName release];
    [subgroups release];
    [records release];
    [super dealloc];
}

- (void) setParentGroup: (Group *)group
{
    parentGroup = group;
}

- (Group *) parentGroup
{
    return parentGroup;
}

- (void) addRecord: (Record *)rec
{
    [records addObject: rec];
    [rec setGroup: self];
}

- (void) addGroup: (Group *)group
{
    [subgroups addObject: group];
    [group setParentGroup: self];
}

- (void) removeRecord: (Record *)rec
{
    if([records containsObject: rec]) {
        [rec setGroup: nil];
        [records removeObject: rec];
    }
}

- (void) removeGroup: (Group *)group
{
    if([subgroups containsObject: group]) {
        [group setParentGroup: nil];
        [subgroups removeObject: group];
    }
}

- (NSString *) name
{
    return fullName;
}

- (NSArray *) subgroups
{
    return subgroups;
}

- (NSArray *) records
{
    return records;
}

- (NSArray *) deepRecords
{
    NSMutableArray *ret =[[NSMutableArray alloc] init];
    int subgroupCount = [subgroups count];
    
    [ret addObjectsFromArray: records];
    
    for(int i = 0; i < subgroupCount; ++i) {
        Group *g = [subgroups objectAtIndex: i];
        [ret addObjectsFromArray: [g deepRecords]];
    }
    
    return [ret autorelease];
}

@end
