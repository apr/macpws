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

#import "Controller.h"


@implementation Controller

- (void) dealloc
{
    [db release];
    [outlineDataSource release];
    [super dealloc];
}

- (void) awakeFromNib
{
    NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];

    [nc addObserver: self selector: @selector(textDidChange:) 
        name: NSControlTextDidChangeNotification object: titleField];
    [nc addObserver: self selector: @selector(textDidChange:) 
        name: NSControlTextDidChangeNotification object: usernameField];
    [nc addObserver: self selector: @selector(textDidChange:) 
        name: NSControlTextDidChangeNotification object: passwordField];
    
    [nc addObserver: self selector: @selector(outlineSelectionDidChange:)
        name: NSOutlineViewSelectionDidChangeNotification
        object: recordsOutline];
        
    [mainWindow setDelegate: self];
    [recordsOutline setDelegate: self];

    outlineDataSource = [[OutlineDataSource alloc] init];
    [recordsOutline setDataSource: outlineDataSource];
    
    [self setDatabase: nil];
    
    dirty = NO;
    dirtyIsNew = NO;
}

- (void) setDatabase: (Database *)database
{
    [database retain];
    [db release];
    db = database;
    [outlineDataSource resetDatabase: db];
}

- (IBAction) cancelRecordEdit: (id)sender
{
    Record *r = [self selectedRecord];
    
    if(dirtyIsNew) {
        [db deleteRecord: r];
        [recordsOutline reloadData];
        r = nil;
    }
    
    if(r) {
        [self displayRecord: r];
    }
    
    [self markCurrentRecordClean];
    [self clearForm];
    [self disableForm];
}

- (IBAction) changePassword: (id)sender
{
    NSString *newPass = [newPasswordController runForWindow: mainWindow];
    
    if(newPass) {
        NSError *err;
        if(![db saveWithNewKey: newPass error: &err]) {
            NSAlert *alert = [NSAlert alertWithError: err];
            [alert runModal];
        }
    }
}

- (IBAction) copyPassword: (id)sender
{
}

- (IBAction) createNewFolder: (id)sender
{
    [newGroupNameField setStringValue: @""];
    [NSApp beginSheet: newGroupSheet modalForWindow: mainWindow
        modalDelegate: self didEndSelector: NULL contextInfo: nil];
}

- (IBAction) createNewRecord: (id)sender
{
    [newRecordTitleField setStringValue: @""];
    [NSApp beginSheet: newRecordSheet modalForWindow: mainWindow
        modalDelegate: self didEndSelector: NULL contextInfo: nil];
}

- (IBAction) doneCreatingNewRecord: (id)sender
{
    [newRecordSheet orderOut: nil];
    [NSApp endSheet: newRecordSheet];
    
    if(sender == newRecordOK) {
        NSString *title = [newRecordTitleField stringValue];
        Group *group = [self selectedGroup];
        Record *rec = [db createNewRecordWithTitle: title inGroup: group];
        
        if(group) {
            [recordsOutline reloadItem: group reloadChildren: YES];
            [recordsOutline expandItem: group];
        } else {
            [recordsOutline reloadData];
        }

        // Select the newly created row.
        NSIndexSet *indexSet = [NSIndexSet
            indexSetWithIndex: [recordsOutline rowForItem: rec]];
        [recordsOutline selectRowIndexes: indexSet byExtendingSelection: NO];
        
        [self markCurrentRecordDirty];
        dirtyIsNew = YES;
        
        // Position the curson in the username field, so that the user
        // can seamlessly edit the record.
        [[usernameField window] makeFirstResponder: usernameField];
    }
}

- (IBAction) doneCreatingNewGroup: (id)sender
{
    [newGroupSheet orderOut: nil];
    [NSApp endSheet: newGroupSheet];
    
    if(sender == newGroupOK) {
        NSString *name = [newGroupNameField stringValue];
        Group *parent = [self selectedGroup];
        Group *group = [db createNewGroupWithName: name parentGroup: parent];
        
        if(parent) {
            [recordsOutline reloadItem: parent reloadChildren: YES];
            [recordsOutline expandItem: parent];
        } else {
            [recordsOutline reloadData];
        }
        
        // Select the newly created group.
        NSIndexSet *indexSet = [NSIndexSet
            indexSetWithIndex: [recordsOutline rowForItem: group]];
        [recordsOutline selectRowIndexes: indexSet byExtendingSelection: NO];
    }
}

- (NSString *) askForNewFile: (NSString *)title
{
    NSSavePanel *savePanel = [NSSavePanel savePanel];
    
    [savePanel setTitle: title];
    
    if([savePanel runModal] == NSOKButton) {
        return [savePanel filename];
    }
    
    return nil;
}

- (IBAction) createNewSafe: (id)sender
{    
    NSString *filename = [self askForNewFile: @"New File"];
    
    if(!filename) {
        // The user has cancelled the operation.
        return;
    }
    
    NSString *pass = [newPasswordController runForWindow: mainWindow];
    
    if(!pass) {
        // The user has cancelled the operation.
        return;
    }

    NSError *err;
    Database *newDb = [Database createEmpty: filename withKey: pass error: &err];

    if(newDb) {
        [self setDatabase: newDb];
        [mainWindow setTitle: filename];
        [recordsOutline reloadData];
        [mainWindow makeKeyAndOrderFront: nil];
    } else {
        NSAlert *alert = [NSAlert alertWithError: err];
        [alert runModal];
    }
}

- (IBAction) deleteItem: (id)sender
{
    Record *record = [self selectedRecord];
    Group *group = [self selectedGroup];
    BOOL saveDb = NO;
    
    if(record) {
        if([self confirmRecordDelete: record]) {
            [db deleteRecord: record];
            saveDb = YES;
        }
    } else if(group) {
        if([self confirmGroupDelete: group]) {
            [db deleteGroup: group];
            saveDb = YES;
        }
    }
    
    // Refresh the outline.
    if(group && record) {
        [recordsOutline reloadItem: group reloadChildren: YES];
    } else {
        [recordsOutline reloadData];
    }
    
    if(saveDb) {
        NSError *err;
        if(![db save: &err]) {
            NSAlert *alert = [NSAlert alertWithError: err];
            [alert runModal];
        }
    }
}

- (IBAction) openSafe: (id)sender
{
    NSString *filename = [self askForFile];
    
    if(!filename) {
        // The user has cancelled the operation.
        return;
    }
    
    NSString *key = [askPasswordController runForWindow: mainWindow];
    
    if(!key) {
        // The user has cancelled the operation.
        return;
    }
    
    NSError *err;
    Database *newDb = [Database readFromFile: filename withKey: key error: &err];

    if(newDb) {
        [self setDatabase: newDb];
        [recordsOutline reloadData];    
        [mainWindow setTitle: filename];
        [mainWindow makeKeyAndOrderFront: nil];
    } else {
        NSAlert *alert = [NSAlert alertWithError: err];
        [alert runModal];
    }
}

- (NSString *) askForFile
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];

    [openPanel setAllowsMultipleSelection: NO];
    [openPanel setCanChooseDirectories: NO];

    if([openPanel runModalForTypes: nil] != NSOKButton) {
        return nil;
    }

    NSArray *filesToOpen = [openPanel filenames];
    NSAssert([filesToOpen count] == 1, @"Expected to get only one file to open");
    
    return [filesToOpen objectAtIndex: 0];
}

- (IBAction) saveRecord: (id)sender
{
    Record *r = [self selectedRecord];
    
    NSAssert(r, @"No record was selected when requesting a save");
    
    if(![[titleField stringValue] isEqualToString: [r title]]) {
        [r setTitle: [titleField stringValue]];
        [recordsOutline reloadItem: r];
    }
    
    if(![[usernameField stringValue] isEqualToString: [r username]]) {
        [r setUsername: [usernameField stringValue]];
    }
    
    if(![[passwordField stringValue] isEqualToString: [r password]]) {
        [r setPassword: [passwordField stringValue]];
    }
    
    if(![[notesField string] isEqualToString: [r notes]]) {
        [r setNotes: [notesField string]];
    }
    
    NSError *err;
    if(![db save: &err]) {
        NSAlert *alert = [NSAlert alertWithError: err];
        [alert runModal];
    }
    
    [self markCurrentRecordClean];
}

- (IBAction) showPassword: (id)sender
{
}

- (IBAction) handleSaveAs: (id)sender
{
    NSLog(@"save as");
}

- (void) clearForm
{
    [titleField setStringValue: @""];
    [usernameField setStringValue: @""];
    [passwordField setStringValue: @""];
    [notesField setString: @""];
}

- (void) markCurrentRecordDirty
{
    if(!dirty) {
        dirty = YES;
        [saveButton setEnabled: YES];
        [cancelButton setEnabled: YES];
    }
}

- (void) markCurrentRecordClean
{
    if(dirty) {
        dirty = NO;
        dirtyIsNew = NO;
        [saveButton setEnabled: NO];
        [cancelButton setEnabled: NO];
    }
}

- (void) displayRecord: (Record *)record
{
    [titleField setStringValue: [record title]];
    [usernameField setStringValue: [record username]];
    [passwordField setStringValue: [record password]];
    [notesField setString: [record notes]];
}

- (void) textDidChange: (NSNotification *)aNotification
{
    [self markCurrentRecordDirty];
}

- (void) outlineSelectionDidChange: (NSNotification *)notification
{
    Record *r = [self selectedRecord];
    
    if(r) {
        [self enableForm];
        [self displayRecord: r];
    } else {
        [self disableForm];
        [self clearForm];
    }
}

- (Record *) selectedRecord
{
    id item = [recordsOutline itemAtRow: [recordsOutline selectedRow]];
    
    if(item && [item isKindOfClass: [Record class]]) {
        return item;
    }
    
    return nil;
}

- (Group *) selectedGroup
{
    id item = [recordsOutline itemAtRow: [recordsOutline selectedRow]];
    
    if(item && [item isKindOfClass: [Group class]]) {
        return item;
    }
    
    if(item) {
        // parentForItem: was officially introduced in 10.5 and so the compiler
        // reports a warning if compiled on an onlder version of Mac OS X.
        // Despite the compiler's complain the selector seems to work.
        return [recordsOutline parentForItem: item];
    }
    
    return nil;
}

- (void) disableForm
{
    [titleField setEnabled: NO];
    [usernameField setEnabled: NO];
    [passwordField setEnabled: NO];
    [notesField setEditable: NO];
}

- (void) enableForm
{
    [titleField setEnabled: YES];
    [usernameField setEnabled: YES];
    [passwordField setEnabled: YES];
    [notesField setEditable: YES];
}

- (BOOL) confirmRecordDelete: (Record *)record
{
    NSAlert *alert = [NSAlert alertWithMessageText: @"Delete the record?"
        defaultButton: @"OK" alternateButton: @"Cancel"
        otherButton: nil informativeTextWithFormat:
            @"Record '%@' will be deleted. Deleted records cannot be restored.",
            [record name]];

    return [alert runModal] == NSAlertDefaultReturn;
}

- (BOOL) confirmGroupDelete: (Group *)group
{
    NSAlert *alert = [NSAlert alertWithMessageText: @"Delete the group?"
        defaultButton: @"OK" alternateButton: @"Cancel"
        otherButton: nil informativeTextWithFormat:
            @"Group '%@' will be deleted. "
            @"All records within the group will be deleted without "
            @"possibility of restore.", [group name]];

    return [alert runModal] == NSAlertDefaultReturn;
}

- (BOOL) windowShouldClose: (id)sender
{
    [self setDatabase: nil];
    [recordsOutline reloadData];
    return YES;
}

// Will not allow a change is outline selection if some record is being edited.
- (BOOL) outlineView: (NSOutlineView *)outlineView shouldSelectItem: (id)item
{
    if(dirty) {
        NSBeep();
    }
    
    return !dirty;
}

- (BOOL) validateMenuItem: (NSMenuItem *)item
{
    // Always show the 'New Safe' and 'Open Safe' menus.
    return (item == newSafeMenu) || (item == openSafeMenu) ||
        [mainWindow isVisible];
}

@end
