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

#import "OutlineDataSource.h"


@implementation OutlineDataSource

- (id) init
{
    if(!(self = [super init])) {
        return nil;
    }
    
    db = nil;
    
    return self;
}

- (void) resetDatabase: (Database *)database
{
    [database retain];
    [db release];
    db = database;
}

- (void) dealloc
{
    [db release];
    [super dealloc];
}

- (int) outlineView: (NSOutlineView *)outlineView
        numberOfChildrenOfItem: (id)item
{
    if(item == nil) {
        return db ? [[db rootObjects] count] : 0;
    }
    
    if(![item isKindOfClass: [Group class]]) {
        return 0;
    }
    
    int groupCount = [[item subgroups] count];
    int recordCount = [[item records] count];

    return groupCount + recordCount;
}

- (BOOL) outlineView: (NSOutlineView *)outlineView
         isItemExpandable: (id)item
{
    return [item isKindOfClass: [Group class]];
}

- (id) outlineView: (NSOutlineView *)outlineView
       child: (int)index
       ofItem: (id)item
{
    if(item == nil) {
        return db ? [[db rootObjects] objectAtIndex: index] : nil;
    }

    if(![item isKindOfClass: [Group class]]) {
        return nil;
    }
    
    int n_groups = [[item subgroups] count];
    
    if(index < n_groups) {
        return [[item subgroups] objectAtIndex: index];
    } else {
        return [[item records] objectAtIndex: (index - n_groups)];
    }
}

- (id) outlineView: (NSOutlineView *)outlineView
       objectValueForTableColumn: (NSTableColumn *)tableColumn
       byItem: (id)item
{
    return [item name];
}

@end
