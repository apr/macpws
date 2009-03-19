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

#import "Database.h"
#import "Record.h"

#import "db/dbio.h"
#import "db/db_reader.h"
#import "db/db_writer.h"
#import "db/exception.h"
#import "db/util.h"

static NSMutableArray *buildObjects(pws::pws_db &db)
{
    NSMutableArray *ret = [NSMutableArray new];
    NSMutableDictionary *groups = [NSMutableDictionary new];
    
    for(int i = 0; i < db.num_records(); ++i) {
        pws::pws_record &r = db.get_record_by_index(i);
        Record *rec = [[Record alloc] initWithRecord: &r];
        NSString *g_name = [rec fullGroupName];
        
        if([g_name length] == 0) {
            [ret addObject: rec];
        } else {
            Group *g = [groups objectForKey: g_name];
            
            if(g == nil) {
                g = [[Group alloc] initWithName: g_name];
                [groups setObject: g forKey: g_name];
                [ret addObject: g];
                [g release];
            }

            [g addRecord: rec];
        }
        
        [rec release];
    }
    
    [groups release];
    
    return [ret autorelease];
}

static NSError *convertExceptionToError(const pws::pws_io_exception &ex)
{
    NSString *descrip = [NSString stringWithUTF8String: ex.what()];
    NSArray *objArray = [NSArray arrayWithObjects: descrip, nil];
    NSArray *keyArray = [NSArray arrayWithObjects: NSLocalizedDescriptionKey,
        nil];
    NSDictionary *eDict = [NSDictionary dictionaryWithObjects: objArray
        forKeys: keyArray];
    return [[[NSError alloc] initWithDomain: PwsErrorDomain
        code: ex.error_code() userInfo: eDict] autorelease];
}


@implementation Database

+ (Database *) createEmpty: (NSString *)path withKey: (NSString *)password
                    error: (NSError **)outError
{
    pws::pws_db *database = new pws::pws_db(0x0305);
    Database *ret = [[Database alloc] initWithPath: path
                                      key: password
                                      database: database];
    
    if(![ret save: outError]) {
        [ret release];
        return nil;
    }
    
    return [ret autorelease];
}

+ (Database *) readFromFile: (NSString *)path withKey: (NSString *)password
                    error: (NSError **)outError
{
    try {
        pws::scoped_ptr<pws::db_reader> reader(
            pws::create_reader([path UTF8String], [password UTF8String]));
        pws::pws_db *database = reader->read();
        Database *ret = [[Database alloc] initWithPath: path
                                          key: password
                                          database: database];
        return [ret autorelease];
    } catch(pws::pws_io_exception ex) {
        if(outError) {
            *outError = convertExceptionToError(ex);
        }
    }
    
    return nil;
}


- (id) initWithPath: (NSString *)path key: (NSString *)k
            database: (pws::pws_db *)database
{
    if(!(self = [super init])) {
        return nil;
    }
    
    db = database;
    filename = [path retain];
    key = [k retain];
    objects = [buildObjects(*db) retain];
    
    return self;
}

- (void) dealloc
{
    delete db;
    [filename release];
    [key release];
    [super dealloc];
}

- (BOOL) save: (NSError **)outError
{
    try {
        pws::scoped_ptr<pws::db_writer> writer(pws::create_writer(*db));
        writer->write(*db, [filename UTF8String], [key UTF8String]);
        return YES;
    } catch(pws::pws_io_exception ex) {
        if(outError) {
            *outError = convertExceptionToError(ex);
        }
    }
    
    return NO;
}

- (BOOL) saveWithNewKey: (NSString *)password error: (NSError **)outError
{
    [password retain];
    [key release];
    key = password;
    return [self save: outError];
}

- (NSString *) filename
{
    return filename;
}

- (Record *) createNewRecordWithTitle: (NSString *)title inGroup: (Group *)group
{
    pws::pws_record &db_r = db->add_record([title UTF8String], "");
    
    if(group) {
        db_r.set_group([[group name] UTF8String]);
    }
    
    Record *r = [[Record alloc] initWithRecord: &db_r];

    if(group) {
        [group addRecord: r];
    } else {
        [objects addObject: r];
    }

    return [r autorelease];
}

- (Group *) createNewGroupWithName: (NSString *)name parentGroup: (Group *)parent
{
    // TODO the name is incorrent, it should be the full name formed
    // using the parent's full name.
    Group *ret = [[Group alloc] initWithName: name];
    
    if(parent) {
        [parent addGroup: ret];
    } else {
        [objects addObject: ret];
    }
    
    return [ret autorelease];
}

- (void) deleteRecord: (Record *)record
{
    Group *group = [record group];

    db->delete_record(*[record dbRecord]);

    if(group) {
        [group removeRecord: record];
    } else {
        [objects removeObject: record];
    }
}

- (void) deleteGroup: (Group *)group
{
    Group *parentGroup = [group parentGroup];
    NSArray *records = [group deepRecords];
    
    for(int i = 0; i < [records count]; ++i) {
        [self deleteRecord: [records objectAtIndex: i]];
    }
    
    if(parentGroup) {
        [parentGroup removeGroup: group];
    } else {
        [objects removeObject: group];
    }
}

- (NSArray *) rootObjects
{
    return objects;
}

@end
