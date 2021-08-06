//
//  NSError+AskarError.m
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/6/21.
//

#import "NSError+AskarError.h"
#import "NSDictionary+JSON.h"
#import "askar_wallet.h"

static NSString *const AskarErrorDomain = @"AskarErrorDomain";

@implementation NSError (AskarError)

+ (NSError *)errorFromAskarError:(ErrorCode)error {
    NSMutableDictionary *userInfo = [NSMutableDictionary new];
    if (error != 0) {
        const char * error_json_p;
        askar_get_current_error(&error_json_p);
        NSString *errorDetailsJson = [NSString stringWithUTF8String:error_json_p];

        NSDictionary *errorDetails = [NSDictionary fromString:errorDetailsJson];
        [userInfo setValue:errorDetails[@"message"] forKey:@"message"];
        [userInfo setValue:errorDetails[@"backtrace"] forKey:@"askar_backtrace"];
    }

    return [NSError errorWithDomain:AskarErrorDomain code:error userInfo:userInfo];
}

@end
