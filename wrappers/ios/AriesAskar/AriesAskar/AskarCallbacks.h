//
//  AskarCallbacks.h
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/6/21.
//

#import <Foundation/Foundation.h>
#import "askar_wallet.h"

extern void AskarWrapperCommonCallback(int64_t callbackId, ErrorCode err);

@interface AskarCallbacks : NSObject

+ (AskarCallbacks *)sharedInstance;

- (int64_t)createCommandHandleFor:(id)callback;

- (void)complete:(void (^)(NSError *))completion
       forHandle:(int64_t)handle
         ifError:(ErrorCode)ret;

@end
