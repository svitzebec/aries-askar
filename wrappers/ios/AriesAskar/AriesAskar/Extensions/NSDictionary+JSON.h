//
//  NSDictionary+JSON.h
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/6/21.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSDictionary (JSON)

- (NSString*) toString;
+ (NSDictionary*) fromString:(NSString*) s;
- (BOOL) contains:(NSDictionary*) dict;

@end

NS_ASSUME_NONNULL_END
