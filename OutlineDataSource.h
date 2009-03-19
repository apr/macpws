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
#import "Database.h"

// A data source for the records outline. Adapts the Database to the
// outline data source interface.
@interface OutlineDataSource : NSObject {
    Database *db;
}

- (id) init;
- (void) dealloc;

- (void) resetDatabase: (Database *)database;

// Outline view data source interface.
- (int) outlineView: (NSOutlineView *)outlineView
        numberOfChildrenOfItem: (id)item;

- (BOOL) outlineView: (NSOutlineView *)outlineView
         isItemExpandable: (id)item;

- (id) outlineView: (NSOutlineView *)outlineView
       child: (int)index
       ofItem: (id)item;

- (id) outlineView: (NSOutlineView *)outlineView
       objectValueForTableColumn: (NSTableColumn *)tableColumn
       byItem: (id)item;

@end
