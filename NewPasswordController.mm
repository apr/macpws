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

#import "NewPasswordController.h"


@implementation NewPasswordController

- (void) awakeFromNib
{
    password = nil;
}

- (void) dealloc
{
    [password release];
    [super dealloc];
}

- (NSString *) runForWindow: (NSWindow *)window
{
    // Reset the password field, it will be filled later when the user
    // closes the panel.
    [password release];
    password = nil;

    // Clear the password form.
    [errorField setHidden: YES];
    [newPasswordField setStringValue: @""];
    [newPasswordConfirmationField setStringValue: @""];
    [[newPasswordField window] makeFirstResponder: newPasswordField];

    [panel center];
    [panel makeKeyAndOrderFront: nil];
    [NSApp runModalForWindow: panel];
    
    return password;
}

- (IBAction) doneEnteringPassword: (id)sender
{
    if(sender == okButton) {
        NSString *newPass = [newPasswordField stringValue];
        NSString *newPassConf = [newPasswordConfirmationField stringValue];
        
        if(![newPass isEqual: newPassConf]) {
            [errorField setHidden: NO];
            return;
        }
        
        password = [newPass retain];
    }
    
    [panel orderOut: nil];
    [NSApp stopModal];
}

@end
