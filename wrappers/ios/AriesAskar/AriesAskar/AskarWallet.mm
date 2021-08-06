//
//  AskarWallet.mm
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/5/21.
//

#import "askar_wallet.h"
#import "AskarWallet.h"
#import "AskarCallbacks.h"
#import <Foundation/Foundation.h>

@implementation AskarWallet

+ (void)sessionUpdate:(SessionHandle)sessionHandle operation:(SInt32)operation category:(NSString *)category name:(NSString *)name value:(NSString *)value tags:(NSString *)tags expiry:(SInt64)expiry completion:(void (^)(NSError *))completion {
    [completion copy];
    ErrorCode ret;
    int64_t handle = [[AskarCallbacks sharedInstance] createCommandHandleFor:completion];

    // TODO: - Call here the session update method
    //    ret = askar_session_update(0, 0, "", "", "", "", 0, NULL, 0);

    [[AskarCallbacks sharedInstance] complete:completion forHandle:handle ifError:ret];
}

@end
