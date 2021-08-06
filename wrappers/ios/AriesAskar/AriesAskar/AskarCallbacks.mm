//
//  AskarCallbacks.mm
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/6/21.
//

#import "AskarCallbacks.h"
#import <Foundation/Foundation.h>
#import "askar_wallet.h"
#import "NSError+AskarError.h"

static NSString *commandCallbackKey = @"commandCallback";

@interface AskarCallbacks ()

@property(strong, readwrite) NSMutableDictionary *commandCompletions;
@property int32_t commandHandleCounter;
@property(strong, readwrite) NSRecursiveLock *globalLock;

@end

@implementation AskarCallbacks

+ (AskarCallbacks *)sharedInstance {
    static AskarCallbacks *instance = nil;
    static dispatch_once_t dispatch_once_block;

    dispatch_once(&dispatch_once_block, ^{
        instance = [AskarCallbacks new];
    });

    return instance;
}

- (AskarCallbacks *)init {
    self = [super init];
    if (self) {
        self.commandHandleCounter = 0;
        self.commandCompletions = [[NSMutableDictionary alloc] init];
        self.globalLock = [NSRecursiveLock new];
    }
    return self;
}

- (int64_t)createCommandHandleFor:(id)callback {
    NSNumber *handle = nil;

    @synchronized (self.globalLock) {
        handle = [NSNumber numberWithInt:self.commandHandleCounter];
        self.commandHandleCounter++;

        NSMutableDictionary *dict = [NSMutableDictionary new];
        dict[commandCallbackKey] = [callback copy];

        self.commandCompletions[handle] = dict;
    }
    return (int64_t) [handle integerValue];
}

- (void)complete:(void (^)(NSError *))completion
       forHandle:(int64_t)handle
         ifError:(ErrorCode)ret {
    if (ret != 0) {
        [self deleteCommandHandleFor:handle];
        dispatch_async(dispatch_get_main_queue(), ^{
            completion([NSError errorFromAskarError:ret]);
        });
    }
}

- (id)commandCompletionFor:(int64_t)handle {
    NSNumber *key = [NSNumber numberWithInt:handle];
    id val = nil;
    @synchronized (self.globalLock) {
        NSMutableDictionary *dict = (NSMutableDictionary *) [self.commandCompletions objectForKey:key];
        val = [dict objectForKey:@"commandCallback"];
    }
    return val;
}

- (void)deleteCommandHandleFor:(int64_t)handle {
    NSNumber *key = [NSNumber numberWithInt:handle];
    @synchronized (self.globalLock) {
        if ([self.commandCompletions objectForKey:key]) {
            [self.commandCompletions removeObjectForKey:key];
        }
    }
}

@end

void AskarWrapperCommonCallback(int64_t xcommand_handle, ErrorCode err) {
    id block = [[AskarCallbacks sharedInstance] commandCompletionFor:xcommand_handle];
    [[AskarCallbacks sharedInstance] deleteCommandHandleFor:xcommand_handle];

    void (^completion)(NSError *) = (void (^)(NSError *)) block;

    NSError *error = [NSError errorFromAskarError:err];

    if (completion) {
        dispatch_async(dispatch_get_main_queue(), ^{
            completion(error);
        });
    }
}
