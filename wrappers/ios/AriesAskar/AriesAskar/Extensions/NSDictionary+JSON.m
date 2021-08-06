//
//  NSDictionary+JSON.m
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/6/21.
//

#import "NSDictionary+JSON.h"

@implementation NSDictionary (JSON)

- (NSString*) toString
{
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:self
                                                       options:kNilOptions
                                                         error:&error];

    if (!jsonData)
    {
        return nil;
    }
    else
    {
        return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
}

+ (NSDictionary*) fromString:(NSString*) s
{
    NSError *error;
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:[NSData dataWithBytes:[s UTF8String]
                                                                                length:[s length]]
                                                         options:kNilOptions
                                                           error: &error];
    return dict;
}

- (BOOL) contains:(NSDictionary*) dict
{
    for (NSString *key in [dict allKeys])
    {
        id object = [self objectForKey:key];
        if ([object isKindOfClass:[NSDictionary class]])
        {
            [[self objectForKey:key] contains:(NSDictionary *)object];
        }
        else if ([self objectForKey:key] != nil
                 && ![[self objectForKey:key] isEqual:[dict objectForKey:key]])
        {
            return NO;
        }
    }
    return YES;
}

@end
