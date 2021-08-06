//
//  NSError+AskarError.h
//  AriesAskar
//
//  Created by Alonso Yoshio Alvarez Tengan on 8/6/21.
//

#import <Foundation/Foundation.h>
#import "askar_wallet.h"

NS_ASSUME_NONNULL_BEGIN

@interface NSError (AskarError)

+ (NSError*) errorFromAskarError:(ErrorCode) error;

@end

NS_ASSUME_NONNULL_END
