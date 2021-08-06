//
//  AskarWallet.hpp
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/5/21.
//

#import "AskarTypes.h"
#import <Foundation/Foundation.h>

@interface AskarWallet : NSObject

/// Add/Update/Delete wallet records.
+ (void)sessionUpdate:(SessionHandle)sessionHandle
            operation:(SInt32)operation
             category:(NSString *)category
                 name:(NSString *)name
                value:(NSString *)value
                 tags:(NSString *)tags
               expiry:(SInt64)expiry
           completion:(void (^)(NSError *error))completion;

@end
