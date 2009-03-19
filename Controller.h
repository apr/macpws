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
#import "AskPasswordController.h"
#import "NewPasswordController.h"
#import "OutlineDataSource.h"

@interface Controller : NSObject
{
    IBOutlet NSWindow *mainWindow;
    IBOutlet NSButton *cancelButton;
    IBOutlet NSTextView *notesField;
    IBOutlet NSTextField *passwordField;
    IBOutlet NSOutlineView *recordsOutline;
    IBOutlet NSButton *saveButton;
    IBOutlet NSButton *showPasswordSwitch;
    IBOutlet NSTextField *titleField;
    IBOutlet NSTextField *usernameField;
    
    IBOutlet NSWindow *newRecordSheet;
    IBOutlet NSButton *newRecordOK;
    IBOutlet NSTextField *newRecordTitleField;
    
    IBOutlet NSWindow *newGroupSheet;
    IBOutlet NSButton *newGroupOK;
    IBOutlet NSTextField *newGroupNameField;
    
    IBOutlet AskPasswordController *askPasswordController;
    IBOutlet NewPasswordController *newPasswordController;
    
    IBOutlet NSMenuItem *newSafeMenu;
    IBOutlet NSMenuItem *openSafeMenu;
    
    Database *db;
    OutlineDataSource *outlineDataSource;
    
    BOOL dirty; // YES if the current record has been edited.
    BOOL dirtyIsNew; // YES if the current record was just created
}

- (void) dealloc;

- (void) awakeFromNib;

- (void) setDatabase: (Database *)database;

- (IBAction) cancelRecordEdit: (id)sender;
- (IBAction) changePassword: (id)sender;
- (IBAction) copyPassword: (id)sender;
- (IBAction) createNewFolder: (id)sender;
- (IBAction) createNewRecord: (id)sender;
- (IBAction) doneCreatingNewRecord: (id)sender;
- (IBAction) doneCreatingNewGroup: (id)sender;
- (IBAction) createNewSafe: (id)sender;
- (IBAction) deleteItem: (id)sender;
- (IBAction) openSafe: (id)sender;
- (IBAction) saveRecord: (id)sender;
- (IBAction) showPassword: (id)sender;
- (IBAction) handleSaveAs: (id)sender;

// Delegate methods.
- (BOOL) windowShouldClose: (id)sender;
- (BOOL) outlineView: (NSOutlineView *)outlineView shouldSelectItem: (id)item;

- (void) clearForm;
- (void) displayRecord: (Record *)record;

// Returns currently selected record or nil if no record is selected.
- (Record *) selectedRecord;

// Returns a group of currently selected record or nil if the record is on
// the top level or if no record is selected.
- (Group *) selectedGroup;

- (void) markCurrentRecordDirty;
- (void) markCurrentRecordClean;

- (void) disableForm;
- (void) enableForm;

- (void) textDidChange: (NSNotification *)notification;
- (void) outlineSelectionDidChange: (NSNotification *)notification;

// Returns YES if confirmed.
- (BOOL) confirmRecordDelete: (Record *)record;
- (BOOL) confirmGroupDelete: (Group *)group;

// Runs the open file panel and returns the choosen name or nil if the user
// cancelled the dialog.
- (NSString *) askForFile;

// Runs the save file panel and returns the choosen name or nil if the user
// cancelled the dialog.
- (NSString *) askForNewFile: (NSString *)title;

- (BOOL) validateMenuItem: (NSMenuItem *)item;

@end
